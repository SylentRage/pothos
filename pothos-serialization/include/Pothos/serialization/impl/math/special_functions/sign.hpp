//  (C) Copyright John Maddock 2006.
//  (C) Copyright Johan Rade 2006.
//  (C) Copyright Paul A. Bristow 2011 (added changesign).

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef POTHOS_MATH_TOOLS_SIGN_HPP
#define POTHOS_MATH_TOOLS_SIGN_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <Pothos/serialization/impl/math/tools/config.hpp>
#include <Pothos/serialization/impl/math/special_functions/math_fwd.hpp>
#include <Pothos/serialization/impl/math/special_functions/detail/fp_traits.hpp>

namespace Pothos{ namespace math{ 

namespace detail {

  // signbit

#ifdef POTHOS_MATH_USE_STD_FPCLASSIFY
    template<class T> 
    inline int signbit_impl(T x, native_tag const&)
    {
        return (std::signbit)(x);
    }
#endif

    template<class T> 
    inline int signbit_impl(T x, generic_tag<true> const&)
    {
        return x < 0;
    }

    template<class T> 
    inline int signbit_impl(T x, generic_tag<false> const&)
    {
        return x < 0;
    }

    template<class T> 
    inline int signbit_impl(T x, ieee_copy_all_bits_tag const&)
    {
        typedef POTHOS_DEDUCED_TYPENAME fp_traits<T>::type traits;

        POTHOS_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);
        return a & traits::sign ? 1 : 0;
    }

    template<class T> 
    inline int signbit_impl(T x, ieee_copy_leading_bits_tag const&)
    {
        typedef POTHOS_DEDUCED_TYPENAME fp_traits<T>::type traits;

        POTHOS_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);

        return a & traits::sign ? 1 : 0;
    }

    // Changesign

    template<class T>
    inline T (changesign_impl)(T x, generic_tag<true> const&)
    {
        return -x;
    }

    template<class T>
    inline T (changesign_impl)(T x, generic_tag<false> const&)
    {
        return -x;
    }


    template<class T>
    inline T changesign_impl(T x, ieee_copy_all_bits_tag const&)
    {
        typedef POTHOS_DEDUCED_TYPENAME fp_traits<T>::sign_change_type traits;

        POTHOS_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);
        a ^= traits::sign;
        traits::set_bits(x,a);
        return x;
    }

    template<class T>
    inline T (changesign_impl)(T x, ieee_copy_leading_bits_tag const&)
    {
        typedef POTHOS_DEDUCED_TYPENAME fp_traits<T>::sign_change_type traits;

        POTHOS_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);
        a ^= traits::sign;
        traits::set_bits(x,a);
        return x;
    }


}   // namespace detail

template<class T> int (signbit)(T x)
{ 
   typedef typename detail::fp_traits<T>::type traits;
   typedef typename traits::method method;
   // typedef typename boost::is_floating_point<T>::type fp_tag;
   typedef typename tools::promote_args_permissive<T>::type result_type;
   return detail::signbit_impl(static_cast<result_type>(x), method());
}

template <class T>
inline int sign POTHOS_NO_MACRO_EXPAND(const T& z)
{
   return (z == 0) ? 0 : (Pothos::math::signbit)(z) ? -1 : 1;
}

template <class T> typename tools::promote_args_permissive<T>::type (changesign)(const T& x)
{ //!< \brief return unchanged binary pattern of x, except for change of sign bit. 
   typedef typename detail::fp_traits<T>::sign_change_type traits;
   typedef typename traits::method method;
   // typedef typename boost::is_floating_point<T>::type fp_tag;
   typedef typename tools::promote_args_permissive<T>::type result_type;

   return detail::changesign_impl(static_cast<result_type>(x), method());
}

template <class T, class U>
inline typename tools::promote_args_permissive<T, U>::type 
   copysign POTHOS_NO_MACRO_EXPAND(const T& x, const U& y)
{
   POTHOS_MATH_STD_USING
   typedef typename tools::promote_args_permissive<T, U>::type result_type;
   return (Pothos::math::signbit)(static_cast<result_type>(x)) != (Pothos::math::signbit)(static_cast<result_type>(y)) 
      ? (Pothos::math::changesign)(static_cast<result_type>(x)) : static_cast<result_type>(x);
}

} // namespace math
} // namespace boost


#endif // BOOST_MATH_TOOLS_SIGN_HPP


