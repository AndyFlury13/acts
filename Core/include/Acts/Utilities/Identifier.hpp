// This file is part of the Acts project.
//
// Copyright (C) 2016-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// Identifier.h, Acts project
///////////////////////////////////////////////////////////////////

#pragma once
#ifdef ACTS_CORE_IDENTIFIER_PLUGIN
#include ACTS_CORE_IDENTIFIER_PLUGIN
#else

#define IDENTIFIER_TYPE unsigned long long
#define IDENTIFIER_DIFF_TYPE long long

#include <string>

/// @class Identifier
///
/// minimum implementation of an Identifier,
/// please use the ACTS_CORE_IDENTIFIER_PLUGING in to use instead if
/// another type of Identifier is needed
///
class Identifier
{
public:
  ///----------------------------------------------------------------
  /// Define public typedefs
  ///----------------------------------------------------------------
  typedef Identifier           id_type;
  typedef IDENTIFIER_TYPE      value_type;
  typedef IDENTIFIER_DIFF_TYPE diff_type;
  typedef IDENTIFIER_TYPE      size_type;

  typedef enum {
    NBITS    = sizeof(value_type) * 8,  // bits per byte
    MAX_BIT  = (static_cast<value_type>(1) << (NBITS - 1)),
    ALL_BITS = ~(static_cast<value_type>(0))
  } bit_defs;

  ///----------------------------------------------------------------
  /// Constructors
  ///----------------------------------------------------------------

  /// Default constructor
  Identifier();

  /// Constructor from value_type
  ///
  /// @param value is the identifier value
  explicit Identifier(value_type value);

  /// Copy constructor
  ///
  /// @param other is the source identifier
  Identifier(const Identifier& other);

  ///----------------------------------------------------------------
  /// Modifications
  ///----------------------------------------------------------------
  ///
  /// @param value is the modification parameter
  Identifier&
  operator|=(value_type value);

  /// @param value is the modification parameter
  Identifier&
  operator&=(value_type value);

  ///----------------------------------------------------------------
  /// Assignment operator
  ///----------------------------------------------------------------
  /// @param old is the assigment parameter
  Identifier&
  operator=(const Identifier& old);

  /// @param value is the assigment parameter
  Identifier&
  operator=(value_type value);

  ///----------------------------------------------------------------
  /// Cast operators to value @todo to bool
  operator value_type() const { return m_id; }
  value_type
  value() const
  {
    return m_id;
  }

  ///----------------------------------------------------------------
  /// Comparison operators
  ///----------------------------------------------------------------
  /// @param other is the comparison parameter
  bool
  operator==(const Identifier& other) const;

  /// @param other is the comparison parameter
  bool
  operator!=(const Identifier& other) const;

  /// @param other is the comparison parameter
  bool
  operator<(const Identifier& other) const;

  /// @param other is the comparison parameter
  bool
  operator>(const Identifier& other) const;

  /// @param other is the comparison parameter
  bool
  operator<=(const Identifier& other) const;

  /// @param other is the comparison parameter
  bool
  operator>=(const Identifier& other) const;

  /// Check if id is in a valid state
  /// @return boolen for the valid state
  bool
  is_valid() const;

private:
  //----------------------------------------------------------------
  // The compact identifier data.
  //----------------------------------------------------------------
  value_type m_id;

  typedef enum {
    // max_value = 0xFFFFFFFFFFFFFFFFULL
    max_value = ~(static_cast<value_type>(0))
  } max_value_type;
};
//-----------------------------------------------

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

// Constructors
//-----------------------------------------------
inline Identifier::Identifier() : m_id(max_value)
{
}

//-----------------------------------------------
inline Identifier::Identifier(const Identifier& other) : m_id(other.m_id)
{
}

//-----------------------------------------------
inline Identifier::Identifier(value_type value) : m_id(value)
{
}

// Modifications
//-----------------------------------------------

inline Identifier&
Identifier::operator=(const Identifier& other)
{
  if (&other != this) {
    m_id = other.m_id;
  }
  return (*this);
}

inline Identifier&
Identifier::operator=(value_type value)
{
  m_id = value;
  return (*this);
}

inline Identifier&
Identifier::operator|=(value_type value)
{
  m_id |= value;
  return (*this);
}

inline Identifier&
Identifier::operator&=(value_type value)
{
  m_id &= value;
  return (*this);
}

// Comparison operators
//----------------------------------------------------------------
inline bool
Identifier::operator==(const Identifier& other) const
{
  return (m_id == other.m_id);
}

//----------------------------------------------------------------
inline bool
Identifier::operator!=(const Identifier& other) const
{
  return (m_id != other.m_id);
}

//-----------------------------------------------
inline bool
Identifier::operator<(const Identifier& other) const
{
  return (m_id < other.m_id);
}

//-----------------------------------------------
inline bool
Identifier::operator>(const Identifier& other) const
{
  return (m_id > other.m_id);
}

//-----------------------------------------------
inline bool
Identifier::operator<=(const Identifier& other) const
{
  return (m_id <= other.m_id);
}

//-----------------------------------------------
inline bool
Identifier::operator>=(const Identifier& other) const
{
  return (m_id >= other.m_id);
}

//----------------------------------------------------------------
inline bool
Identifier::is_valid() const
{
  return (!(max_value == m_id));
}

#endif  // ACTS_CORE_IDENTIFIER_PLUGIN