/*******************************************************************\

Module: Symbolic Execution

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <simplify_expr.h>

#include "goto_symex.h"

/*******************************************************************\

Function: goto_symext::symex_catch

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_symext::symex_catch()
{
  // there are two variants: 'push' and 'pop'
  const goto_programt::instructiont &instruction=*cur_state->source.pc;

  if(instruction.targets.empty()) // The second catch, pop from the stack
  {
    // Copy the exception before pop
    goto_symex_statet::exceptiont exception=stack_catch.top();

    // Pop from the stack
    stack_catch.pop();

    // Increase the program counter
    cur_state->source.pc++;

    if(exception.has_throw_target)
    {
      // the next instruction is always a goto
      const goto_programt::instructiont &goto_instruction=*cur_state->source.pc;

      // Update target
      goto_instruction.targets.pop_back();
      goto_instruction.targets.push_back(exception.throw_target);

      exception.has_throw_target = false;
    }
  }
  else // The first catch, push it to the stack
  {
    goto_symex_statet::exceptiont exception;

    // copy targets
    const irept::subt &exception_list=
      instruction.code.find("exception_list").get_sub();

    assert(exception_list.size()==instruction.targets.size());

    // Fill the map with the catch type and the target
    unsigned i=0;
    for(goto_programt::targetst::const_iterator
        it=instruction.targets.begin();
        it!=instruction.targets.end();
        it++, i++)
    {
      exception.catch_map[exception_list[i].id()]=*it;
      exception.catch_order[exception_list[i].id()]=i;
    }

    // Stack it
    stack_catch.push(exception);

    // Increase the program counter
    cur_state->source.pc++;
  }
}

/*******************************************************************\

Function: goto_symext::symex_throw

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_symext::symex_throw()
{
  const goto_programt::instructiont &instruction= *cur_state->source.pc;

  // Get the list of exceptions thrown
  const irept::subt &exceptions_thrown=
    instruction.code.find("exception_list").get_sub();

  // Get the list of catchs
  goto_symex_statet::exceptiont* except=&stack_catch.top();

  // We check before iterate over the throw list to save time:
  // If there is no catch, we return an error
  if(!except->catch_map.size())
    exception_error(goto_symex_statet::exceptiont::NOCATCH,
        exceptions_thrown.begin()->id());

  if(!handle_rethrow(exceptions_thrown, instruction))
    return;

  // It'll be used for catch ordering when throwing
  // a derived object with multiple inheritance
  unsigned old_id_number=-1, new_id_number=0;

  for(irept::subt::const_iterator
      e_it=exceptions_thrown.begin();
      e_it!=exceptions_thrown.end();
      e_it++)
  {
    // Search for a catch with a matching type
    goto_symex_statet::exceptiont::catch_mapt::const_iterator
      c_it=except->catch_map.find(e_it->id());

    // Do we have a catch for it?
    if(c_it!=except->catch_map.end())
    {
      // We do!

      // Get current catch number and update if needed
      new_id_number = (*except->catch_order.find(e_it->id())).second;

      if(new_id_number < old_id_number)
        update_throw_target(except,c_it);

      // Save old number id
      old_id_number = new_id_number;
    }
    else // We don't have a catch for it
    {
      // If it's a pointer, we must look for a catch(void*)
      if(e_it->id().as_string().find("_ptr") != std::string::npos)
      {
        // It's a pointer!

        // Do we have an void*?
        c_it=except->catch_map.find("void_ptr");

        if(c_it!=except->catch_map.end() && !except->has_throw_target)
          update_throw_target(except, c_it); // Make the jump to void*
      }
      else
      {
        // Do we have an ellipsis?
        c_it=except->catch_map.find("ellipsis");

        if(c_it!=except->catch_map.end() && !except->has_throw_target)
          update_throw_target(except, c_it);
      }
    }
  }

  if(!except->has_throw_target)
  {
    // An un-caught exception. Error
    exception_error(goto_symex_statet::exceptiont::NOCATCH,
      e_it->id().as_string());
  }

  // save last throw for rethrow handling
  last_throw = &instruction;
}

/*******************************************************************\

Function: goto_symext::update_throw_target

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_symext::exception_error(
    goto_symex_statet::exceptiont::ERROR error,
    const irep_idt &id)
{
  switch(error)
  {

  case goto_symex_statet::exceptiont::NOCATCH:
  {
    // An un-caught exception. Error
    const std::string &msg="Throwing an exception of type " +
      id.as_string() + " but there is not catch for it.";
    claim(false_exprt(), msg);

    break;
  }

  case goto_symex_statet::exceptiont::NOTALLOWED:
  {
    break;
  }

  case goto_symex_statet::exceptiont::NOLASTTHROW:
  {
    const std::string &msg="Trying to re-throw without last exception.";
    claim(false_exprt(), msg);
    break;
  }

  }
}


/*******************************************************************\

Function: goto_symext::update_throw_target

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_symext::update_throw_target(goto_symex_statet::exceptiont* except,
  goto_symex_statet::exceptiont::catch_mapt::const_iterator c_it)
{
  except->has_throw_target=true;
  except->throw_target=(*c_it).second;
}

/*******************************************************************\

Function: goto_symext::handle_throw_decl

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_symext::handle_throw_decl(goto_symex_statet::framet* frame,
  const irep_idt &id)
{

}

/*******************************************************************\

Function: goto_symext::handle_rethrow

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool goto_symext::handle_rethrow(irept::subt exceptions_thrown,
  const goto_programt::instructiont instruction)
{
  // throw without argument, we must rethrow last exception
  if(!exceptions_thrown.size())
  {
    if(last_throw != NULL && last_throw->code.find("exception_list").get_sub().size())
    {
      // get exception from last throw
      irept::subt::const_iterator e_it=last_throw->code.find("exception_list").get_sub().begin();

      // update current state exception list
      instruction.code.find("exception_list").get_sub().push_back((*e_it));

      return true;
    }
    else
    {
      exception_error(goto_symex_statet::exceptiont::NOLASTTHROW);
      return false;
    }
  }
  return true;
}

/*******************************************************************\

Function: goto_symext::symex_throw_decl

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_symext::symex_throw_decl()
{

}
