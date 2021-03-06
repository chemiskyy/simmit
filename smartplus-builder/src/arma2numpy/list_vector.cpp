/* This file is part of arma2numpy.
 
 arma2numpy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 arma2numpy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with arma2numpy.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include <iostream>
#include <vector>
#include <memory>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <smartplus/Libraries/Identification/constants.hpp>
#include <smartplus/Libraries/Identification/parameters.hpp>

namespace bp = boost::python;

namespace arma2numpy {

template<typename T> std::vector<T> py_list_to_std_vector(const bp::object& iterable)
{
    return std::vector<T>( bp::stl_input_iterator<T>( iterable ),
                            bp::stl_input_iterator<T>() );
}
    
std::vector<double> py_list_to_std_vector_double(const bp::object& iterable)
{
    return py_list_to_std_vector<double>(iterable);
}
    
std::vector<int> py_list_to_std_vector_int(const bp::object& iterable)
{
    return py_list_to_std_vector<int>(iterable);
}
    
std::vector<std::string> py_list_to_std_vector_string(const bp::object& iterable)
{
    return py_list_to_std_vector<std::string>(iterable);
}

std::vector<smart::constants> py_list_to_std_vector_constants(const bp::object& iterable)
{
    return py_list_to_std_vector<smart::constants>(iterable);
}

std::vector<smart::parameters> py_list_to_std_vector_parameters(const bp::object& iterable)
{
    return py_list_to_std_vector<smart::parameters>(iterable);
}
    
/// @brief Transfer ownership to a Python object.  If the transfer fails,
///        then object will be destroyed and an exception is thrown.
template <typename T> boost::python::object transfer_to_python(T* t)
{
    // Transfer ownership to a smart pointer, allowing for proper cleanup
    // incase Boost.Python throws.
    std::unique_ptr<T> ptr(t);
    
    // Create a functor with a call policy that will have Boost.Python
    // manage the new object, then invoke it.
    bp::object object = bp::make_function([t]() { return t; },
                                          bp::return_value_policy<bp::manage_new_object>(),
                                          boost::mpl::vector<T*>())();
    
    // As the Python object now has ownership, release ownership from
    // the smart pointer.
    ptr.release();
    return object;
}
    
template <class T> bp::list std_vector_to_py_list(const std::vector<T> &vector) {
    bp::list list;
    for (auto const & x : vector) {
        list.append(x);
    }
    return list;
}
    
template <class T> bp::list std_vector_to_py_list_class(const std::vector<T> &vector) {
    bp::list list;
    for (auto r : vector) {
        T *x = new T(r);
        list.append(transfer_to_python<T>(x));
    }
    return list;
}

boost::python::list std_vector_to_py_list_double(const std::vector<double> &vector)
{
    return std_vector_to_py_list<double>(vector);
}

boost::python::list std_vector_to_py_list_int(const std::vector<int> &vector)
{
    return std_vector_to_py_list<int>(vector);
}
    
boost::python::list std_vector_to_py_list_string(const std::vector<std::string> &vector)
{
    return std_vector_to_py_list<std::string>(vector);
}

boost::python::list std_vector_to_py_list_constants(const std::vector<smart::constants> &vector)
{
    return std_vector_to_py_list_class<smart::constants>(vector);
}
    
boost::python::list std_vector_to_py_list_parameters(const std::vector<smart::parameters> &vector)
{
    return std_vector_to_py_list_class<smart::parameters>(vector);
}
    
} //end of namespace arma2numpy