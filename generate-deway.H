# ifndef GENERATE_DEWAY_H
# define GENERATE_DEWAY_H

template <class Node>
void generate_deway(Node * r, string parent_str, nat_t num_child,
		    ostream & output)
{
  output << "Node " << parent_str << " \"";
  if (r->get_key() == '$')
    output << "\\";  
  output << r->get_key() << "\"\n";

  nat_t counter = 0;

  r->for_each_child([&counter, &output, &parent_str, num_child] (auto c)
		    {
		      stringstream s;
		      s << parent_str << '.' << num_child;
		      generate_deway(c, s.str(), counter, output);
		      ++counter;
		    });
}

template <class Node>
void generate_deway(Node * r, ostream & output)
{
  output << "Root \"" << r->get_key() << "\"\n";

  nat_t counter = 0;

  r->for_each_child([&counter, &output] (auto c)
		    {
		      generate_deway(c, "0", counter, output);
		      ++counter;
		    });
}

# endif // GENERATE_DEWAY_H
