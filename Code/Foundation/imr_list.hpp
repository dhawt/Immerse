/***************************************************************************\
  File: IMR_List.hpp
  Description: Templated list classes.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_LIST__HPP
#define __IMR_LIST__HPP

// Include stuff:
#include <stdlib.h>
#include <string.h>

// List class with named items ---
// For use with classes with the Is(), SetName(), and GetName() member functions:
template <class IMR_NLT>
class IMR_NamedList
    {
    private:
      int Num_Items, Max_Items;
      IMR_NLT *Items;

    public:
      IMR_NamedList();
      IMR_NamedList(int max);
      ~IMR_NamedList() { delete [] Items; };
      void Init(int max);
      inline void Reset(void)
          {
          delete [] Items;
          Max_Items = Num_Items = 0;
           };

      IMR_NLT *Add_Item(char *Name);
      void Delete_Item(char *Name);

      IMR_NLT *Get_Item(char *Name, int *Index);
      inline int Get_Item_Index(char *Name) { int tmp; Get_Item(Name, &tmp); return tmp; };
      
      inline IMR_NLT &operator[] (int Index) { return Items[Index]; };
      inline int Get_Num_Items(void) { return Num_Items; };
      inline int Get_Max_Items(void) { return Max_Items; };
     };      

/***************************************************************************\
  Default constructor.
\***************************************************************************/
template <class IMR_NLT>
IMR_NamedList<IMR_NLT>::IMR_NamedList(): Num_Items(0), Items(0), Max_Items(0)
{
 }

/***************************************************************************\
  Constructor with size argument.
\***************************************************************************/
template <class IMR_NLT>
IMR_NamedList<IMR_NLT>::IMR_NamedList(int max): Max_Items(max), Items(new IMR_NLT[max])
{
 }

/***************************************************************************\
  Initializes the list.
\***************************************************************************/
template <class IMR_NLT>
void IMR_NamedList<IMR_NLT>::Init(int max)
{
Max_Items = max;
Items = new IMR_NLT[max];
if (!Items) Max_Items = 0;
 }

/***************************************************************************\
  Adds an item to the list with the specified name.
\***************************************************************************/
template <class IMR_NLT>
IMR_NLT *IMR_NamedList<IMR_NLT>::Add_Item(char *Name)
{
// One more item:
++ Num_Items;

// If we have too many items, return null:
if (Num_Items > Max_Items)
    {
    Num_Items = Max_Items;
    return NULL;
     }

// Set the name of the new item:
Items[Num_Items - 1].Set_Name(Name);

// And return a pointer to it:
return &Items[Num_Items - 1];
 }

/***************************************************************************\
  Removes the item with the specified name from the list (if it exists).
\***************************************************************************/
template <class IMR_NLT>
void IMR_NamedList<IMR_NLT>::Delete_Item(char *Name)
{
int ItemIndex;

// Get an index to the item:
if (!Get_Item(Name, &ItemIndex)) return;

// Shift all the items over the item we are deleting:
memmove((void *)&Items[ItemIndex], 
        (void *)&Items[ItemIndex + 1], 
        sizeof(IMR_NLT) * (Num_Items - ItemIndex - 1));

// One less item:
-- Num_Items;
 }

/***************************************************************************\
  Returns a pointer to the item with the specified name.
\***************************************************************************/
template <class IMR_NLT>
IMR_NLT *IMR_NamedList<IMR_NLT>::Get_Item(char *Name, int *index)
{
// Search each item in the list:
for (int item = 0; item < Num_Items; item ++)
    if (Items[item].Is(Name)) 
        {
        if (index) *index = item;
        return &Items[item];
         }

// No match was found, so return null:
return NULL;
 }

// List class with id'd items ---
// For use with classes with the Is(), SetID(), and GetID() member functions:
template <class IMR_IDLT>
class IMR_IDList
    {
    private:
      int Num_Items, Max_Items;
      IMR_IDLT *Items;

    public:
      IMR_IDList();
      IMR_IDList(int max);
      ~IMR_IDList() { delete [] Items; };
      void Init(int max);
      inline void Reset(void)
          {
          delete [] Items;
          Max_Items = Num_Items = 0;
           };

      IMR_IDLT *Add_Item(int ID);
      void Delete_Item(int ID);

      IMR_IDLT *Get_Item(int ID, int *Index);
      inline int Get_Item_Index(int ID) { int tmp; Get_Item(ID, &tmp); return tmp; };
      
      inline IMR_IDLT &operator[] (int Index) { return Items[Index]; };
      inline int Get_Num_Items(void) { return Num_Items; };
     };      

/***************************************************************************\
  Default constructor.
\***************************************************************************/
template <class IMR_IDLT>
IMR_IDList<IMR_IDLT>::IMR_IDList(): Num_Items(0), Items(0), Max_Items(0)
{
 }

/***************************************************************************\
  Constructor with size argument.
\***************************************************************************/
template <class IMR_IDLT>
IMR_IDList<IMR_IDLT>::IMR_IDList(int max): Max_Items(max), Items(new IMR_IDLT[max])
{
 }

/***************************************************************************\
  Initializes the list.
\***************************************************************************/
template <class IMR_IDLT>
void IMR_IDList<IMR_IDLT>::Init(int max)
{
Max_Items = max;
Items = new IMR_IDLT[max];
if (!Items) Max_Items = 0;
 }

/***************************************************************************\
  Adds an item to the list with the specified id.
\***************************************************************************/
template <class IMR_IDLT>
IMR_IDLT *IMR_IDList<IMR_IDLT>::Add_Item(int ID)
{
// One more item:
++ Num_Items;

// If we have too many items, return null:
if (Num_Items > Max_Items)
    {
    Num_Items = Max_Items;
    return NULL;
     }

// Set the name of the new item:
Items[Num_Items - 1].Set_ID(ID);

// And return a pointer to it:
return &Items[Num_Items - 1];
 }

/***************************************************************************\
  Removes the item with the specified ID from the list (if it exists).
\***************************************************************************/
template <class IMR_IDLT>
void IMR_IDList<IMR_IDLT>::Delete_Item(int ID)
{
int ItemIndex;

// Get an index to the item:
if (!Get_Item(ID, &ItemIndex)) return;

// Shift all the items over the item we are deleting:
memmove((void *)&Items[ItemIndex], 
        (void *)&Items[ItemIndex + 1], 
        sizeof(IMR_IDLT) * (Num_Items - ItemIndex - 1));

// One less item:
-- Num_Items;
 }

/***************************************************************************\
  Returns a pointer to the item with the specified ID.
\***************************************************************************/
template <class IMR_IDLT>
IMR_IDLT *IMR_IDList<IMR_IDLT>::Get_Item(int ID, int *index)
{
// Search each item in the list:
for (int item = 0; item < Num_Items; item ++)
    if (Items[item].Is(ID)) 
        {
        if (index) *index = item;
        return &Items[item];
         }

// No match was found, so return null:
return NULL;
 }

#endif
