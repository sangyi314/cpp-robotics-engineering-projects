#include "csv_logger.hpp"

#include <iomanip>
#include <locale>

bool write_csv_header (std::ostream& output)
{
    output.imbue(std::locale::classic());
    output << std::fixed << std::setprecision(6);

     output << "sequence,timestamp_us,raw_ax,raw_ay,raw_az,"
              "raw_gx,raw_gy,raw_gz,filtered_ax\n";

    return static_cast<bool>(output);

}

bool write_csv_row (std::ostream& output , const ProcessedData& data)
{
    const auto& raw = data.raw ;

    output << raw.sequence << ','
           << raw.timestamp_us << ','
           << raw.imu.ax << ','
           << raw.imu.ay << ','
           << raw.imu.az << ','
           << raw.imu.gx << ','
           << raw.imu.gy << ','
           << raw.imu.gz << ','
           << data.filtered_ax << '\n';

    return static_cast<bool>(output);



}