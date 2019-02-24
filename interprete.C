# include <cassert>
# include <cstring>

# include <stdio.h>
# include <string.h>
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>

# include <iostream>
# include <tuple>
# include <stdexcept>
# include <string>
# include <sstream>

# include <readline/readline.h>
# include <readline/history.h>

using namespace std;

# include <list.H>
# include <nodesdef.H>
# include <stack.H>

using namespace Designar;

# include <helpers.H>
# include <expnode-sol.H>
# include <generate-deway.H>
# include <dictnode.H>

Cnode * root = new Cnode('-');

string get_prompt(size_t i)
{
    stringstream s;
    s << i << " $ ";
    return s.str();
}

void extract_word (const string word, const string word_to_search, Cnode * tree)
{
    size_t def = 0, pos_i = 0, pos_f = 0;
    while (true)
    {
        def = word.find(word_to_search, def) + 1;
        if (def == 0)
            break;
        pos_i = word.find (",",def) + 1;
        pos_f = word.find (",", pos_i) - pos_i;
        string word_to_insert(word, pos_i, pos_f);
        tree->insert_word(word_to_insert);
    }
}

char * completion(const char * prefix, int state) 
{
    static vector<string> matches;
    static size_t index = 0;
    string text(prefix);
    if (state == 0) 
    {
        matches.clear();
        index = 0;
        size_t pos = text.size();
        for (auto word : root->words())
            if (word.size() >= pos and word.compare(0, pos, text) == 0)
                matches.push_back(word);
    }
  
    return (index >= matches.size()) ? nullptr : 
                                                                 strdup(matches[index++].c_str());
}

char ** find_match(const char* prefix, int start, int end) 
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(prefix, completion);
}

int main()
{
    vector<string> reserv_words = {"void","int","isvoid","pair","fst","snd",
                                                               "def","var","neg","add","mul","divmod",
                                                               "let","ifgreater","fun","call"};
    for (auto word : reserv_words)
        root->insert_word(word);
      
    rl_completer_word_break_characters = "<, ";
    rl_attempted_completion_function = find_match;
    cout << "Command line for PR3 programming language\n"
             << "If yo want to quit, type exit and enter\n\n";
  
    size_t counter = 0;

    Enviroment env;

    while (true)
    {
        string prompt = get_prompt(counter);
        char * line = readline(prompt.c_str());
      
        if (line == nullptr)
	        break;

        string prog = line;
      
        if (prog == "exit")
	        break;

        Exp * expr = nullptr;

        try
	    {
	        expr = parse(remove_whites(line));

	        if (expr == nullptr)
	            continue;
	  
	        Exp * result = expr->eval(env);
	        cout << result->to_string() << endl;
            extract_word(prog,"def",root);
            extract_word(prog,"fun",root);
	        delete result;
            delete expr;
	        ++counter;
	    }
        catch(const domain_error & e)
	    {
	        delete expr;
	        cout << "Error: " << e.what() << endl;
	    }
        catch(const logic_error & e)
	    {
	        cout << "Error: " << e.what() << endl;
        }

        add_history(line);
        free(line);
    }
    return 0;
}
