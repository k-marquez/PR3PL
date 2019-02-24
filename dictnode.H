// @author: Alejandro Mujica

using namespace std;

class Cnode : public MTreeNode<char>
{
  static char filter_char(register const char c)
  {
    switch (c)
      {
      case 'á': 
      case 'à': 
      case 'ä': 
      case 'â':
      case 'ã':
      case 'Á': 
      case 'À': 
      case 'Ä': 
      case 'Â':
      case 'Ã':	return 'a'; 

      case 'é': 
      case 'è':
      case 'ë':
      case 'ê':
      case 'É': 
      case 'È':
      case 'Ë':
      case 'Ê':	return 'e'; 

      case 'í': 
      case 'ì':
      case 'ï':
      case 'î':
      case 'Í': 
      case 'Ì':
      case 'Ï':
      case 'Î':	return 'i'; 

      case 'ó':
      case 'ò':
      case 'ö':
      case 'ô':
      case 'õ':
      case 'Ó':
      case 'Ò':
      case 'Ö':
      case 'Ô':
      case 'Õ':	return 'o'; 

      case 'ú': 
      case 'ù':
      case 'ü':
      case 'û':
      case 'Ú': 
      case 'Ù':
      case 'Ü':
      case 'Û':	return 'u'; 

      case 'ñ': 
      case 'Ñ': 
	return 'n'; 

      case 'ç':
      case 'Ç':	return 'c'; 

      case 'A' ... 'Z' : return c + 32;

      case '-':
      case '$':
      case 'a' ... 'z': return c;

      default: throw invalid_argument("Invalid character");
      }
  }

static char fast_filter_char(register const char c)
  {
    switch (c)
      {
      case 'á': 
      case 'à': 
      case 'ä': 
      case 'â':
      case 'ã':
      case 'Á': 
      case 'À': 
      case 'Ä': 
      case 'Â':
      case 'Ã':	return 'a'; 

      case 'é': 
      case 'è':
      case 'ë':
      case 'ê':
      case 'É': 
      case 'È':
      case 'Ë':
      case 'Ê':	return 'e'; 

      case 'í': 
      case 'ì':
      case 'ï':
      case 'î':
      case 'Í': 
      case 'Ì':
      case 'Ï':
      case 'Î':	return 'i'; 

      case 'ó':
      case 'ò':
      case 'ö':
      case 'ô':
      case 'õ':
      case 'Ó':
      case 'Ò':
      case 'Ö':
      case 'Ô':
      case 'Õ':	return 'o'; 

      case 'ú': 
      case 'ù':
      case 'ü':
      case 'û':
      case 'Ú': 
      case 'Ù':
      case 'Ü':
      case 'Û':	return 'u'; 

      case 'ñ': 
      case 'Ñ': 
	return 'n'; 

      case 'ç':
      case 'Ç':	return 'c'; 

      case 'A' ... 'Z' : return c + 32;

      case '-':
      case '$':
      case 'a' ... 'z': 
      default: return c;
      }
  }

  static string filter_string(const string & s)
  {
    string ret_val; ret_val.reserve(50);
    for (char * ptr = (char*) s.c_str(); *ptr; ++ptr)
      ret_val.push_back(filter_char(*ptr));
 
    return ret_val;
  }

  static char * filter_string(char * ptr)
  {
    char * ret = ptr;
    for (; *ptr; ++ptr)
      *ptr = filter_char(*ptr);

    return ret;
  }

public:

  Cnode(const char c) 
  {
    this->get_key() = filter_char(c); 
  }

  char symbol() const noexcept { return get_key(); }

  SLList<Cnode*> children() const 
  {
    SLList<Cnode*> r;
    for_each_child([&r] (MTreeNode<char> * p) { r.append((Cnode*) p); });
    return r;
  }

  string to_str() const 
  {
    string ret(1, symbol());
    children().for_each([&ret] (Cnode * p) { ret += "(" + p->to_str() + ")"; });
    return ret;
  }

private:

  struct W
  {
    const string operator () (Cnode * p) const
    {
      char c = p->symbol();
      return c != '$' ? string(1, p->symbol()) : string("\\$");
    }
  };

public:

  void ntreepic(ostream & out)
  {
    generate_deway(this, out);
  }

  bool is_end_word() const noexcept
  {
    MTreeNode * child = get_first_child();
    if (child == nullptr)
      return false;
    return child->get_key() == '$';
  }

  void mark_end_word()
  {
    assert(not is_end_word());
    insert_child(new Cnode ('$'));
  }

  Cnode * search_child(const char c) const noexcept
  {
    for (MTreeNode * child = get_first_child(); child != nullptr; 
         child = child->get_right_sibling())
      if (child->get_key() == c)
        return (Cnode*) child;

    return nullptr;
  }

  Cnode * greater_child(const char c) const noexcept
  {
    for (MTreeNode * child = get_first_child(); child != nullptr; 
         child = child->get_right_sibling())
      if (child->get_key() > c)
        return (Cnode*) child;
    return nullptr;
  }

  // retorna el nodo situado al final del prefijo prefix
  tuple<const Cnode*, const char*> search_prefix(const char * prefix) const
  {
    assert(prefix != nullptr);

    if (*prefix == '\0')
      return make_tuple(this, "");

    Cnode * p = search_child(fast_filter_char(*prefix));
    if (p == nullptr) // encontró *prefix?
      return make_tuple(this, prefix); // No! ==> llegamos hasta aquí

    return p->search_prefix(prefix + 1);
  }

  const Cnode * search_word(const char * word) const
  {
    if (*word == '\0')
      return is_end_word() ? this : nullptr;

    Cnode * ptr = search_child(fast_filter_char(*word));
    if (ptr == nullptr)
      return nullptr;

    return ptr->search_word(word + 1);
  }

  bool contains(const string & word) const noexcept
  {
    return search_word(word.c_str());
  }

  Cnode * insert_child(Cnode * child)
  {
    assert(not children().exists([child] (Cnode * p) 
				 { return p->symbol() == child->symbol(); }));

    Cnode * sibling = greater_child(child->symbol());
    if (sibling == nullptr)
      this->append_child(child);
    else
      sibling->add_left_sibling(child);

    return child;
  }

  bool insert_word(const string & word) 
  {
    SLList<Cnode*> list;
    try
      {
	tuple<const Cnode*, const char*> p = search_prefix(word.c_str());
	const Cnode * pp = get<0>(p);
	char * rem = const_cast<char*>(get<1>(p));

	if (*rem == '\0')
	  if (pp->is_end_word())
	    return false;
	  else
	    {
	      const_cast<Cnode*>(pp)->mark_end_word();
	      return true;
	    }

	// revisar caracteres válidos del resto de la palabra
	for (char *ptr = rem; *ptr; ptr++)
	  {
	    *ptr = filter_char(*ptr);
	    list.append(new Cnode(*ptr));
	  }

	Cnode * child = 
	  const_cast<Cnode*>(pp)->insert_child(list.remove_first());
	while (not list.is_empty())
	  child = child->insert_child(list.remove_first());

	child->mark_end_word();

	return true;
      }
    catch (invalid_argument & e)
      {
	throw invalid_argument("word contains an invalid character");
      }
  }

  void destroy()
  {
    //    children().for_each([] (Cnode * p) { destroy_tree(p); });
  }

private:

  static string extract_word_from_stack(DynStack<char> & stack)
  {
    string ret_val;
    ret_val.reserve(stack.size());

    char * ptr = &stack.base();
    char * last = &stack.top();
    while (ptr <= last)
      ret_val.append(1, *ptr++);

    return ret_val;
  }

  void words(DynStack<char> & stack, DynArray<string> & l)
  {
    if (symbol() == '$')
      l.append(extract_word_from_stack(stack));

    stack.push(symbol());

    for (MTreeNode<char> * child = get_first_child(); child != nullptr;
	 child = child->get_right_sibling())
      static_cast<Cnode*>(child)->words(stack, l);
 
    stack.pop();
  }

public:

  DynArray<string> words() const
  {
    static DynStack<char> stack(2048);
    assert(stack.is_empty());
    DynArray<string> ret_val;

    children().for_each([&] (Cnode * p) { p->words(stack, ret_val); });

    return ret_val;
  }

  void print_words() const
  {
    words().for_each([] (const string & w) { cout << w << endl; });
  }

  static void clone(Cnode * src, Cnode * tgt)
  {
    SLList<Cnode*> src_children, tgt_children;
    src->for_each_child([&src_children, tgt, &tgt_children] (auto p)
			{
			  Cnode * s = static_cast<Cnode*>(p);
			  auto t = new Cnode(s->symbol());
			  tgt->append_child(t);
			  src_children.append(s);
			  tgt_children.append(t);
			});
    src_children.zip(tgt_children).for_each([] (auto p) 
					    {
					      clone(p.first, p.second);
					    });
  }

  static void clone_alt(MTreeNode<char> * src, MTreeNode<char> * tgt)
  {
    src->for_each_child([tgt] (auto s)
			{
			  tgt->append_child(new Cnode(s->get_key()));
			});
    using It = MTreeNode<char>::ChildrenIterator;
    It it1(*src);
    It it2(*tgt);
    for ( ; it1.has_current() and it2.has_current(); it1.next(), it2.next()) 
      {
	auto p = it1.get_current();
	auto q = it2.get_current();
	clone_alt(p, q);
      }
  }

  Cnode * clon() const
  {
    Cnode * ret = new Cnode(symbol());
    clone_alt(const_cast<Cnode*>(this), ret);
    return ret;
  }
};
