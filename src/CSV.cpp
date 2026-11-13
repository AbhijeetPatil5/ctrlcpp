#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <ctrlcpp/plot/CSV.hpp>

namespace ctrlcpp::plot{

    void writeCSV(const std::string& path,
                  const std::vector<std::string>& headers,
                  const std::vector<std::vector<double>>& columns) {
        if (headers.size() != columns.size()) {
            throw std::runtime_error("writeCSV: Number of headers does not match number of columns.");
        }

        std::ofstream file(path);

        if (!file) {
            throw std::runtime_error("writeCSV: Could not open " + path + " for writing.");
        }

        file << std::setprecision(10);

        const std::size_t numColumns = columns.size();
        const std::size_t numRows = 0;
        if (!columns.empty()) {
            numRows = columns[0].size();
        }

        for (std::size_t j = 0; j < numColumns; ++j) {
            if (j < numColumns - 1){
                file << headers[j] << ",";
            } else {
                file << headers[j] << "\n";
            }
        }

        for (std::size_t i = 0; i < numRows; ++i) {
            for (std::size_t j = 0; j < numColumns; ++j) {
                if ( j < numColumns - 1) {
                    file << columns[j][i] << ",";
                } else {
                    file << columns[j][i] << "\n";
                }
            }
        }
}
}