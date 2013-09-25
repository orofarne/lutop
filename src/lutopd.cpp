#include "Manager.hpp"

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

int
smain(int argc, char *argv[]) {
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    po::options_description desc{"Allowed options"};
    desc.add_options()
        ("help,h", "help message")
        ("config-path,c", po::value< std::vector<std::string> >(), "configuration path")
        ("pool-size,p", po::value< unsigned int >(), "size of worker pool")
        ("worker-lifetime,l", po::value< unsigned int >(), "max worker cycles")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("help")) {
        std::cerr << desc << "\n";
        return 1;
    }

    unsigned int pool_size = vm["pool-size"].as< unsigned int >();
    unsigned int worker_lifetime = vm["worker-liftime"].as< unsigned int >();

    std::vector<std::string> files;

    if(vm.count("config-path")) {
        auto conf_path = vm["config-path"].as< std::vector<std::string> >();
        for(std::string &dir : conf_path) {
            fs::path p(dir);
            if(fs::is_regular_file(p) && (p.extension() == ".lua") && file_size(p) > 0) {
                files.push_back(p.native());
            }
            else if (fs::is_directory(p)) {
                std::for_each(fs::directory_iterator(p), fs::directory_iterator(),
                    [&](fs::directory_entry &de) {
                        fs::path p2 = de.path();
                        if(fs::is_regular_file(p2) && (p2.extension() == ".lua") && file_size(p2) > 0) {
                            files.push_back(p2.native());
                        }
                    }
                );
            }
        }
    }
    else {
        std::cerr << desc << "\n";
        return 1;
    }

    // run it...
    lutop::Manager m{pool_size, worker_lifetime};

    m.loadFiles(files);

    return 0;
}

int
main(int argc, char *argv[]) {
    try {
        return smain(argc, argv);
    }
    catch(std::exception const &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    catch(...) {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
