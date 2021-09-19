#pragma once

#include <typeinfo>
#include <cassert>

#include "rules.hpp"
#include "closure.hpp"
#include "action.hpp"
#include "goto.hpp"
#include "conflicts.hpp"
#include "tokens.hpp"

#ifndef CELLAR_TRACE_PARSER
#define CELLAR_TRACE_PARSER 0
#endif 

#if CELLAR_TRACE_PARSER
#include "../output.hpp"
#endif

namespace cellar
{
    // Should really be hidden/moved from here
    template<typename It>
    struct parse_state
    {
        typedef void(*NextFn)(parse_state&, const std::type_info&);
        std::vector<NextFn> stack;
        tree parse_tree;
        token_stream<It> tokens;

        parse_state(const token_stream<It> & t) : tokens(t) {}
    };

    template<typename State, typename It>
    void parse(parse_state<It> & state);

    template<typename It>
    void parse_success(parse_state<It> &state, const std::type_info&)
    {
        state.parse_tree.success = true;
    }

    template<typename Item>
    struct get_next_token;

    template<typename S, int Id, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id>, 0, Lookahead>>
    {
        using type = token<Lookahead>;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<rule_position<S, rule<Id>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, int Token, typename...Def, typename...Items, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, token<Token, Def...>, Items...>, 0, Lookahead>>
    {
        using type = token<Token>;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<rule_position<S, rule<Id, token<Token, Def...>, Items...>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, typename Item, typename...Items, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, Item, Items...>, 0, Lookahead>>
    {
        using type = empty_tree;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<rule_position<S, rule<Id, Item, Items...>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, typename Item, typename...Items, int Position, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, Item, Items...>, Position, Lookahead>>
    {
        using type = typename get_next_token<rule_position<S, rule<Id, Items...>, Position-1, Lookahead>>::type;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<
            rule_position<S, rule<Id, Item, Items...>, Position, Lookahead>, 
            get_next_token<rule_position<S, rule<Id, Items...>, Position-1, Lookahead>>,
            type>;
    };

    template<typename State, typename Symbols, typename Iterator>
    struct process_goto;

    template<typename State, typename It>
    struct process_goto<State, typeset<>, It>
    {
        static void process(parse_state<It> & state, const std::type_info & type)
        {
            assert(!"Something has gone terribly wrong - unhandled symbol being reduced");
        }
    };

    template<typename State, typename Symbol, typename... Symbols, typename It>
    struct process_goto<State, typeset<Symbol, Symbols...>, It>
    {
        static void process(parse_state<It> & state, const std::type_info & type)
        {
            if(typeid(Symbol) == type)
            {
                // Shift this token
                using NextState = typename goto_<State, Symbol>::type;
                parse<NextState>(state);
            }
            else
            {
                process_goto<State, typeset<Symbols...>, It>::process(state, type);
            }
        }
    };

    template<typename State, typename Symbol, typename It>
    struct process_goto<State, typeset<Symbol>, It>
    {
        // Optimization when there is only one shift type
        // - there is no need to check the type as this should always be true.
        static void process(parse_state<It> & state, const std::type_info & type)
        {
            assert(typeid(Symbol) == type);
            using NextState = typename goto_<State, Symbol>::type;
            parse<NextState>(state);
        }
    };


    /*
        This function has been called when a rule has been reduced.
        The reduced rule is stored in the "rule" parameter.
     */
    template<typename State, typename It>
    void reduce_fn(parse_state<It> & ps, const std::type_info &rule)
    {
        // Find all the gotos for a given state.
        using C = typename closure<State>::type;
        using Gotos = typename build_goto_list<C>::type;
#if CELLAR_TRACE_PARSER
        std::cout << "Reduced to state " << C() << " with gotos = " << Gotos() << std::endl;
#endif
        process_goto<State, Gotos, It>::process(ps, rule);
    }

    // Compute the list of tokens that this state is able to process
    // Anything else is a syntax error
    template<typename Item, typename List>
    struct build_next_token_loop
    {
        using T1 = typename get_next_token<Item>::type;
        using type = typename tree_union<List, T1>::type;

        using profile_tag = build_next_token_list_tag;
        using profile_types = profile<
            Item,
            List,
            get_next_token<Item>,
            tree_union<List, T1>
            >;
    };

    template<typename Closure>
    struct build_next_token_list
    {
        using type = typename forall<Closure, empty_tree, build_next_token_loop>::type;

        using profile_tag = build_next_token_list_tag;
        using profile_types = profile<forall<Closure, empty_tree, build_next_token_loop>>;
    };

    template<typename State, int Token, typename It, 
        bool Shifts = resolve_conflicts<State, Token>::is_shift>
    struct process_token;

    template<typename State, int Token, typename It>
    struct process_token<State, Token, It, true>
    {
        static void process(parse_state<It> & state)
        {
#if CELLAR_TRACE_PARSER
            std::cout << "Shifting" << std::endl;
#endif
            auto node = state.parse_tree.shift(Token, state.tokens.begin().location, state.tokens.size());

            // Write the token into the parse tree
            auto p = node.c_str();
            for(auto ch : state.tokens)
                *p++ = ch;

            state.tokens.lex();
            using S2 = typename shift_action<State, Token>::type;
            state.stack.push_back(&reduce_fn<State, It>);
            return parse<S2>(state);
        }
    };

    template<typename State, typename It>
    struct process_token<State, EndOfStream, It, true>
    {
        static void process(parse_state<It> & state)
        {
            // Parse success
            state.parse_tree.success = true;
        }
    };



    /*
        The reduce case
     */
    template<typename State, int Token, typename It>
    struct process_token<State, Token, It, false>
    {
        static void process(parse_state<It> & state)
        {
            using Reduce = typename resolve_conflicts<State, Token>::type;
            
            using Rule = typename Reduce::rule;
            
#if CELLAR_TRACE_PARSER
            std::cout << "Reducing " << Reduce() << std::endl;
#endif
            if(Rule::length==0)
                state.stack.push_back(&reduce_fn<State, It>);
                
            for(int i=1; i<Rule::length; ++i)
                state.stack.pop_back();
            state.parse_tree.reduce(Rule::id, Rule::length);
            auto fn = state.stack.back();
            fn(state, typeid(typename Reduce::symbol));
        }
    };

    template<typename Closure, typename Tokens, typename It>
    struct process_token_list;

    template<typename State, typename It>
    struct process_token_list<State, empty_tree, It>
    {
        static void process(parse_state<It> & state)
        {
            // Shift the error token onto the tree for debugging purposes
            auto node = state.parse_tree.shift(state.tokens.token(), state.tokens.begin().location, state.tokens.size());

            // Write the token into the parse tree
            auto p = node.c_str();
            for(auto ch : state.tokens)
                *p++ = ch;

            // TODO: Report list of possible tokens
            state.parse_tree.SyntaxError(state.tokens.begin().location);
        }
    };

    template<typename State, int Id, typename L, typename R, typename It>
    struct process_token_list<State, type_tree<token<Id>, L, R>, It>
    {
        static void process(parse_state<It> & state)
        {
            if(state.tokens.token() == Id)
                process_token<State, Id, It>::process(state);
            else if(state.tokens.token() < Id)
                process_token_list<State, L, It>::process(state);
            else
                process_token_list<State, R, It>::process(state);
        }
    };

    template<typename State, int Id, typename It>
    struct process_token_list<State, token<Id>, It>
    {
        static void process(parse_state<It> & state)
        {
            if(state.tokens.token() == Id)
                process_token<State, Id, It>::process(state);
            else
                process_token_list<State, empty_tree, It>::process(state);
        }
    };



    // DELETEME
    template<typename Tree, typename Visitor>
    struct visit_tree
    {
        template<typename...Args>
        static void visit(Args&&... args)
        {
            Visitor::template visit<Tree>(std::forward<Args&&...>(args...));
        }
    };

    template<typename Visitor>
    struct visit_tree<empty_tree, Visitor>
    {
        template<typename...Args>
        static void visit(Args&&... args)
        {
        }
    };

    template<typename H, typename L, typename R, typename Visitor>
    struct visit_tree<type_tree<H, L, R>, Visitor>
    {
        template<typename...Args>
        static void visit(Args&&... args)
        {
            Visitor::template visit<H>(std::forward<Args&&...>(args...));
            visit_tree<L, Visitor>::visit(std::forward<Args&&...>(args...));
            visit_tree<R, Visitor>::visit(std::forward<Args&&...>(args...));
        }
    };

    template<typename State, typename It>
    void parse(parse_state<It> & state)
    {
        // Process one token from the input stream.
        using Closure = typename closure<State>::type;
        using Tokens = typename build_next_token_list<Closure>::type;

#if CELLAR_TRACE_PARSER
        std::cout << "Lookahead token = " << state.tokens.token() << " = ";
        for(auto ch : state.tokens)
            std::cout << ch;
        std::cout << std::endl;
        std::cout << "State = " << State() << std::endl;
        std::cout << "Closure = " << Closure() << std::endl;
        std::cout << "Possible tokens = " << Tokens() << std::endl;
#endif

        process_token_list<State, Tokens, It>::process(state);
    }

    class WholeProgram : public symbol<> {};

    template<typename S>
    struct initial_state
    {
        using type = rule_position<WholeProgram, rule<0, S, token<EndOfStream>>, 0, EndOfStream>;
    };

    template<typename Symbol, typename It>
    void parse(const token_stream<It> &tokens, tree&t)
    {
#if CELLAR_TRACE_PARSER
        std::cout << "\n\n=== Starting parse ===\n";
#endif

        parse_state<It> state(tokens);
        state.parse_tree = std::move(t);
        t.clear();

        using State0 = typename initial_state<Symbol>::type;

        state.tokens.lex();
        state.stack.push_back(parse_success);
        parse<State0>(state);

        t = std::move(state.parse_tree);
    }

    template<typename Lexer, typename Grammar, typename It>
    void parse(It start, It end, tree&t)
    {
        auto tokens = make_lexer<Lexer,linecounter<It>>().tokenize(start, end);
        parse<Grammar>(tokens, t);
    }

    template<typename Grammar, typename It = const char*>
    parser<It> make_parser(const lexer<linecounter<It>> & l)
    {
        return parser<It>(l, parse<Grammar>);
    }

    template<typename Grammar, typename Lexer = typename make_lexer_from_grammar<Grammar>::type, typename It = const char*>
    parser<It> make_parser()
    {
        return make_parser<Grammar>(make_lexer<Lexer, linecounter<It>>());
    }
}
