
#include <armadillo>
#include <boost/python.hpp>
#include <boost/numpy.hpp>
#include <simmit/arma2numpy/numpy_arma.hpp>
#include <simmit/arma2numpy/list_vector.hpp>

#include <smartplus/Libraries/Identification/identification.hpp>
#include <smartplus/Libraries/Identification/constants.hpp>
#include <smartplus/Libraries/Identification/parameters.hpp>
#include <smartplus/Libraries/Identification/read.hpp>
#include <smartplus/Libraries/Identification/script.hpp>
#include <smartplus/Libraries/Identification/optimize.hpp>
#include <simmit/smartplus_wrappers/Libraries/Identification/identification.hpp>

namespace bn = boost::numpy;
namespace bp = boost::python;
using namespace std;
using namespace arma;
using namespace arma2numpy;

namespace smartpy {
    
//This function computes the identifcation of materials parameters for one/multiple homogeneous mixed thermomechanical loading experiment
void identification(const bp::str &simul_type_py, const int &n_param, const int &n_consts, const int &nfiles, const int &ngen, const int &aleaspace, const int &pop_py, const int &ngboys, const int &maxpop, const int &stationnary_cond, const bp::str &path_data_py, const bp::str &path_keys_py, const bp::str &path_results_py, const bp::str &materialfile_py, const bp::str &outputfile_py) {
    
    int apop = 0;
    int spop = 0;
    
    if(aleaspace == 2)
        apop = pop_py;
    else if(aleaspace < 2)
        spop = pop_py;
    
    std::string simul_type = bp::extract<std::string>(simul_type_py);
    std::string path_data = bp::extract<std::string>(path_data_py);
    std::string path_keys = bp::extract<std::string>(path_keys_py);
    std::string path_results = bp::extract<std::string>(path_results_py);
    std::string materialfile = bp::extract<std::string>(materialfile_py);
    std::string outputfile = bp::extract<std::string>(outputfile_py);
    smart::run_identification(simul_type, n_param, n_consts, nfiles, ngen, aleaspace, apop, spop, ngboys, maxpop, stationnary_cond, path_data, path_keys, path_results, materialfile, outputfile);
}

bp::list read_constants_py(const int &nconstants, const int &nfiles) {
    std::vector<smart::constants> consts(nconstants);
    smart::read_constants(nconstants, consts, nfiles);
    return std_vector_to_py_list_constants(consts);
}
    
bp::list read_parameters_py(const int &nparams) {
    std::vector<smart::parameters> params(nparams);
    smart::read_parameters(nparams, params);
    return std_vector_to_py_list_parameters(params);
}
    
//This function will copy the constant files
void copy_constants_py(const bp::list &consts_py, const string &src_path, const string &dst_path) {

    std::vector<smart::constants> consts = py_list_to_std_vector_constants(consts_py);
    smart::copy_constants(consts, src_path, dst_path);
}

//This function will copy the parameters files
void copy_parameters_py(const bp::list &params_py, const string &src_path, const string &dst_path) {
    
    std::vector<smart::parameters> params = py_list_to_std_vector_parameters(params_py);
    smart::copy_parameters(params, src_path, dst_path);
}
    
void apply_constants_py(const bp::list &consts_py, const string &dst_path) {
    
    std::vector<smart::constants> consts = py_list_to_std_vector_constants(consts_py);
    smart::apply_constants(consts, dst_path);
}

void apply_parameters_py(const bp::list &params_py, const string &dst_path) {
    
    std::vector<smart::parameters> params = py_list_to_std_vector_parameters(params_py);
    smart::apply_parameters(params, dst_path);
}
    
double calc_cost(const int &nfiles, const string &data_num_name) {

    //Get the data structures
    std::vector<smart::opti_data> data_exp(nfiles);
    std::vector<smart::opti_data> data_weight(nfiles);
    std::vector<smart::opti_data> data_num(nfiles);
    
    Col<int> weight_types(3);
    vec weight_files = zeros(nfiles);
    vector<vec> weight_cols(nfiles);
    
    smart::read_data_exp(nfiles, data_exp);
    smart::read_data_weights(nfiles, weight_types, weight_files, weight_cols, data_weight, data_exp);
    smart::read_data_num(nfiles, data_exp, data_num);
    
    /// Get the data vectors
    ///Import of the experimental data
    string data_exp_folder="exp_data";
    string data_num_folder="num_data";

    string data_num_name_ext = data_num_name.substr(data_num_name.length()-4,data_num_name.length());
    string data_num_name_root = data_num_name.substr(0,data_num_name.length()-4); //to remove the extension
    
    int sizev = 0;
    for(int i=0; i<nfiles;i++) {
        data_exp[i].import(data_exp_folder);
        data_weight[i].import(data_exp_folder);
        sizev += data_exp[i].ndata * data_exp[i].ninfo;
        
        data_num[i].name = data_num_name_root + "_" + to_string(i+1) + data_num_name_ext;
        data_num[i].import(data_num_folder);
    }
    
    ///Computation of the cost function
    vec vexp = smart::calcV(data_exp, data_exp, nfiles, sizev);
    vec vnum = smart::calcV(data_num, data_exp, nfiles, sizev);
    vec W = smart::calcW(sizev, nfiles, weight_types, weight_files, weight_cols, data_weight, data_exp);

    return smart::calcC(vexp, vnum, W);
}
    
    
} //namepsace smartpy