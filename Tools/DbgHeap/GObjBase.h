#pragma once

namespace GObj
{

// This is a base template class for guarded objects.
// Encapsulates a wrapper around some type/object that needs cleanup.
// Should not be used as-is. Instead use either GObj_T or GRef_T
template <class T>
class GBase_T
	:protected T
{
protected:
	void InternalCleanup()
	{
		if (IsValid())
			T::InternalDestroy();
	}
	void InternalSetNull()
	{
		T::m_Value = T::InternalNullValue();
	}
public:
	// Destroys the guarded object (if valid).
	~GBase_T()
	{
		InternalCleanup();
	}
	// Explicitly destroy the guarded object (if valid) and set it to invalid.
	void Destroy()
	{
		if (IsValid())
		{
			T::InternalDestroy();
			InternalSetNull();
		}
	}
	// Returns the guarded object and set it to invalid without destroying it. Use with care!
	typename T::GuardType Detach()
	{
		typename T::GuardType val = T::m_Value;
		InternalSetNull();
		return val;
	}
	// Destroys the guarded object (if valid) and assign it to a given one.
	void Attach(typename T::GuardType val)
	{
		InternalCleanup();
		T::m_Value = val;
	}
	// Swaps the guarded object.
	void Swap(GBase_T<T>& other)
	{
		swap_t(m_Value, other.m_Value);
	}
	// Checks if the guarded object is valid.
	bool IsValid() const
	{
		return T::InternalNullValue() != T::m_Value;
	}
	// Checks if the guarded object is NOT valid.
	bool IsNull() const
	{
		return T::InternalNullValue() == T::m_Value;
	}
	// returns the guarded object's value.
	typename T::GuardType GetValue() const
	{
		return T::m_Value;
	}
	// returns the guarded object (same as GetValue).
	operator typename T::GuardType() const
	{
		return T::m_Value;
	}
	// Test for validity (same as IsValid)
	operator bool () const
	{
		return IsValid();
	}
	// A more convenient way to access an object for some types (such as smart pointers). NOTE: No test for validity is performed here
	typename T::GuardType operator ->()
	{
		return T::m_Value;
	}
	// If the guarded type is a kind of a pointer - the following is needed for arithmetic operations, (otherwise confusing with operator bool)
	typename T::GuardType operator + (int nVal)
	{
		return T::m_Value + nVal;
	}
	typename T::GuardType operator - (int nVal)
	{
		return T::m_Value - nVal;
	}

	// Exposes a direct access to the guarded object variable. Use with care!
	typename T::GuardType& GetRef()
	{
		return T::m_Value;
	}
};

// This is a common class for simple guard objects, those that don't support referencing, just destruction.
// For such objects it is illegal to either assign one wrapper to another or use copy constructors,
// since in such a case two wrappers will attempt to destroy the object.
// The Attach is in fact equivalent to the assignment operator.
template <class T>
class GObj_T :public GBase_T<T>
{
	// Copy constructor is disabled
	GObj_T(const GObj_T& val);
	// Assignment from another guarded object is disabled
	GObj_T& operator = (const GObj_T& val);
public:
	// Empty constructor. Sets the guarded object to invalid
	GObj_T()
	{
		InternalSetNull();
	}
	// Constructor that receives an object and saves it. (the object may be invalid)
	GObj_T(typename T::GuardType val)
	{
		T::m_Value = val;
	}
	// Destroy the old guarded object (if valid) and save the new one.
	GObj_T& operator = (typename T::GuardType val)
	{
		InternalCleanup();
		T::m_Value = val;
		return *this;
	}
};

// A class for guard objects that support referencing, such as smart pointers.
// Encapsulates objects that support referencing. Means holding a reference to the object requires
// to call an extra function (such as AddRef for smart pointers).
// For such objects it is legal to have multiple wrappers for the the same object.
// The Attach is NOT equivalent to the assignment operator.
template <class T>
class GRef_T :public GBase_T<T> {
protected:
	void InternalAssign(typename T::GuardType val)
	{
		T::m_Value = val;
		if (IsValid())
			T::Reference();
	}
public:
	// Empty constructor. Sets the guarded object to invalid
	GRef_T()
	{
		InternalSetNull();
	}
	// Constructor that receives an object and saves it. (the object may be invalid)
	GRef_T(typename T::GuardType val)
	{
		InternalAssign(val);
	}
	// Constructor that receives a wrapper to the object and saves the object (the object may be invalid)
	GRef_T(const GRef_T& val)
	{
		InternalAssign(val);
	}
	// Constructor that either saves or attaches to the given object. (the object may be invalid)
	GRef_T(typename T::GuardType val, bool bAddRef)
	{
		if (bAddRef)
			InternalAssign(val);
		else
			T::m_Value = val;
	}
	// Destroys the old object (if valid) and saves the new one.
	GRef_T& operator = (typename T::GuardType val)
	{
		// This is needed if you intend to assign an object to itself.
		if (val != T::m_Value)
		{
			InternalCleanup();
			InternalAssign(val);
		}
		return *this;
	}
	// Destroys the old object (if valid) and saves the new one.
	GRef_T& operator = (const GRef_T& val)
	{
		return operator = (val.T::m_Value);
	}
};

// In order to use those guard template classes - usually it's enough to instantiate the
// typedef for GObj_T/GRef_T, but in some cases we'll also want to override those classes
// for some extra-functionality.

// Alas, the C++ rules are stupid enough to generate copy constructor and assignment
// operator automatically, by just copying the memory. Of course this can't be a correct
// behaviour in our case. Because of this we've implemented those two functions explicitly
// in GObj_T/GRef_T classes.

// But unfortunately this is not enough to be absolutely sure the compiler won't ruin that.
// If you override a class and forget to re-write copy constructor/assignment the compiler
// will generate those two functions automatically again, incorrectly of course.

// To avoid such cases we obey the following rule: NEVER inherit the GObj_T class directly.
// Use the following macros instead:
#define INHERIT_GUARD_OBJ_BASE(gnew, gbase, gtype) \
private: \
	gnew(const gnew&); \
	gnew(const gbase&); \
	gnew& operator = (const gnew&); \
	gnew& operator = (const gbase&); \
public: \
	gnew& operator = (gtype val) { gbase::operator = (val); return *this; }

// Use this macro to inherit the GObj_T class. For more info see INHERIT_GUARD_OBJ_BASE
#define INHERIT_GUARD_OBJ(gnew, gbase, gtype) \
	INHERIT_GUARD_OBJ_BASE(gnew, gbase, gtype) \
	gnew() {} \
	gnew(gtype val) :gbase(val) {}

// Use this macro to inherit the GRef_T class. For more info see INHERIT_GUARD_OBJ_BASE
#define INHERIT_GUARD_REF_BASE(gnew, gbase, gtype) \
public: \
	gnew(const gnew& val) :gbase(val) {} \
	gnew(const gbase& val) :gbase(val) {} \
	gnew& operator = (gtype val) { gbase::operator = (val); return *this; } \
	gnew& operator = (const gnew& val) { gbase::operator = (val); return *this; } \
	gnew& operator = (const gbase& val) { gbase::operator = (val); return *this; }

// Use this macro to inherit the GRef_T class. For more info see INHERIT_GUARD_OBJ_BASE
#define INHERIT_GUARD_REF(gnew, gbase, gtype) \
	INHERIT_GUARD_REF_BASE(gnew, gbase, gtype) \
	gnew() {} \
	gnew(gtype val) :gbase(val) {} \
	gnew(gtype val, bool bAddRef) :gbase(val, bAddRef) {}

// Base template class. Declares a member of the provided type
template <class T>
struct GBaseH_Core {
	// The variable for the guarded object (used by GObj_T/GRef_T)
	T m_Value;
	// The declaration of the guarded type (used by GObj_T/GRef_T)
	typedef T GuardType;
};

// Implements InternalNullValue by returning NULL of specified type, applicable for most of the types.
template <class T>
struct GBaseH_Null {
	// NULL is the invalid value for the given type (This is used by GObj_T/GRef_T to test the validity of the object)
	static T InternalNullValue() { return NULL; }
};

// Implements InternalNullValue by returning -1 of specified type, applicable for some special cases, such as file/socket handles.
template <class T>
struct GBaseH_Minus1 {
	// -1 (or alternatively 0xFFFFFFFF) is the invalid value for the given type (This is used by GObj_T/GRef_T to test the validity of the object)
	static T InternalNullValue() { return (T) -1; }
};

// Combines the GBaseH_Core and GBaseH_Null
template <class T>
struct GBaseH_CoreNull :public GBaseH_Core<T>, public GBaseH_Null<T> {
};

// Combines the GBaseH_Core and GBaseH_Minus1
template <class T>
struct GBaseH_CoreMinus1 :public GBaseH_Core<T>, public GBaseH_Minus1<T> {
};

// Implements destruction of the object by delete operator
template <class T>
struct GBaseH_Ptr :public GBaseH_CoreNull<T*> {
	// Implements destruction of the object by delete operator
	void InternalDestroy() { delete m_Value; }
};

// Guards objects allocated via new operator. The destruction is implemented via
// the delete operator.
template <class T>
class GPtr_T :public GObj_T<GBaseH_Ptr<T> >
{
	INHERIT_GUARD_OBJ(GPtr_T, GObj_T<GBaseH_Ptr<T> >, T*)
};

// Implements destruction of the objects ARRAY by delete[] operator
template <class T>
struct GBaseH_Arr :public GBaseH_CoreNull<T*> {
	// Implements destruction of the object by delete operator
	void InternalDestroy() { delete[] m_Value; }
};

// Guards objects allocated via new operator. The destruction is implemented via
// the delete operator.
template <class T>
class GArr_T :public GObj_T<GBaseH_Arr<T> >
{
	INHERIT_GUARD_OBJ(GArr_T, GObj_T<GBaseH_Arr<T> >, T*)
};

}; // namespace GObj
