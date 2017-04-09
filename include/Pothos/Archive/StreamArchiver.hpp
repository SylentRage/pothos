///
/// \file Archive/StreamArchiver.hpp
///
/// Archive implementation on top of streaming interfaces.
///
/// \copyright
/// Copyright (c) 2016 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <Pothos/Config.hpp>
#include <type_traits>
#include <iosfwd>
#include <cstddef> //size_t

namespace Pothos {
namespace Archive {

/*!
 * The output stream archiver serializes types to an output stream.
 */
class POTHOS_API OStreamArchiver
{
public:
    OStreamArchiver(std::ostream &os);

    typedef std::true_type isSave;

    template <typename T>
    void operator&(const T &value);

    template <typename T>
    void operator<<(const T &value);

    void writeBytes(const void *buff, const size_t len);

private:

    std::ostream &os;
    unsigned int ver;
};

/*!
 * The input stream archiver deserializes types from an input stream.
 */
class POTHOS_API IStreamArchiver
{
public:
    IStreamArchiver(std::istream &is);

    typedef std::false_type isSave;

    template <typename T>
    void operator&(T &value);

    template <typename T>
    void operator>>(T &value);

    void readBytes(void *buff, const size_t len);

private:

    std::istream &is;
    unsigned int ver;
};

} //namespace Archive
} //namespace Pothos

#include <Pothos/Archive/Invoke.hpp>

template <typename T>
void Pothos::Archive::OStreamArchiver::operator&(const T &value)
{
    Pothos::serialization::invokeSerialize(*this, const_cast<T &>(value), ver);
}

template <typename T>
void Pothos::Archive::OStreamArchiver::operator<<(const T &value)
{
    *this & value;
}

template <typename T>
void Pothos::Archive::IStreamArchiver::operator&(T &value)
{
    Pothos::serialization::invokeSerialize(*this, value, ver);
}

template <typename T>
void Pothos::Archive::IStreamArchiver::operator>>(T &value)
{
    *this & value;
}
