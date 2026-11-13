#ifndef WRITE_CSV_HPP
#define WRITE_CSV_HPP

#include <string>
#include <vector>

namespace ctrlcpp::plot
{
    void writeCSV(const std::string& path,
                  const std::vector<std::string>& headers,
                  const std::vector<std::vector<double>>& columns);
}

#endif