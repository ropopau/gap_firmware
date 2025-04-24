#ifndef UTILS_H
#define UTILS_H



/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 * WARNING: any const qualifier of @ptr is lost.
 * 
 * Not familiar with container_of() ? This black magic originates from the
 * Linux kernel code. Take a look at the following piece of documentatin
 * for an illustrated example :
 * https://www.kernel.org/doc/Documentation/driver-model/design-patterns.txt
 */
#define container_of(ptr, type, member) ({            \
    const typeof(((type*)0)->member)* __mptr = (ptr); \
    (type*)((char*)__mptr - offsetof(type, member));  \
})



#endif /* UTILS_H */
