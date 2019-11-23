CPIM2
=====

This is a console/text based personal information manager,
i.e. a contact manager, written in standard C (C99)

There are 4 branches which each use a different method
of storing the contacts

master uses cvector_void to store both contacts and attributes.

vector_template uses cvector_contact and cvector_attribute
generated using the [template-like](https://github.com/rswinkle/CPIM2/blob/vector_template/cpim.h#L19) [macros](https://github.com/rswinkle/CPIM2/blob/vector_template/cpim.c#L11) in CVector.

linked_list uses an intrusive doubly linked list and helper
functions and macros based closely on list.h from the Linux
kernel.

sqlite uses a sqlite3 database with a single contacts table.  It
also uses cvector_void for attributes while manipulating a contact.
Because they're arbitrary, attributes are stored as a single text column
in the table, '\n' delimited naturally since that's how they're read from the user,
and there's a function to go from that format to a vector of attributes and vice versa.


I wrote this for the following reasons:

1. To contrast with my really old C PIM project
available here http://www.robertwinkler.com/projects.php

2. As a platform for reaquainting myself with designing
and writing new code.

3. As a reason to improve my C helper libraries and create
more.  Also to have a something to practice using said
libraries.  It's hard to design a library/API without
actually using it.  It also serves as a "real-life" test
suite.  I think real use is the best kind of testing.

4. To increase my Vim knowledge (ok all programming does this)


