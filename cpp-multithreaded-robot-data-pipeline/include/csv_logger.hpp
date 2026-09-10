#pragma once 

#include <ostream>

#include "processed_data.hpp"

bool write_csv_header (std::ostream& output);

bool write_csv_row (std::ostream& output , const ProcessedData& data);

