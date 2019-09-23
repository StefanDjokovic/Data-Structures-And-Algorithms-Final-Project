# General Project guidelines:
Develop a well performing and correct algorithm in C using only libc library without any thread nor parallel programming technique. It will be run on university’s server with different test cases and its correctness, time and memory efficiency will be evaluated. The code is to be written and developed autonomously.
## Theme:
Develop a mechanism of relationship monitoring between entities that can vary overtime. 
The mechanism should be able to add new entities, delete an entity, add relationships between two entities, 
delete existing relationships. Each entity is identified by its name, and every relationship has a verse. 

After the `report` command, the system prints, for each relationship type, the entity that is pointed the most, 
and in case of equal values those entities need to be printed ordered by their name. The application should be optimized 
for a big number of entities and a low number of relationship types.

## Commands:

`addent <id_ent>` : adds a new entity if there is no entity with this name

`delent <id_ent>` : deletes the entity with this name, and if it doesn’t exist it does nothing

`addrel <id_orig> <id_dest> <id_rel>` : adds the relationship of the type specified by <id_rel> from the first entity to the second; 
this happens only if this type of relationship doesn’t exist from the first entity to the second and if the two entities exist

`delrel <id_orig> <id_dest> <id_rel>` : deletes the relationship type specified by <id_rel> from the first to the 
second entity it this exists

`report` : does what was specified in the theme

`end` : ends the software

## Other things to note: 

All identifiers are specified by “” and are case sensitives.
