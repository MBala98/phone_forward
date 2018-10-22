# phone_forward

Program for forwarding phone numbers.
Program has an interface with following commands:
NEW ID - creates a base of redirections with id ID if it doesn't exist and if it does it sets it as a current base.
DEL ID - deletes base of redirections with given ID.
number > number - adds redirection of numbers to a current base.
number ? - gives redirections from given number.
? number - gives redirections on given number.
DEL number - deletes all redirections with number as its prefix.

Program is implemented using trie.

