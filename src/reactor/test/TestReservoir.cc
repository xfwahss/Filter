#include "../include/Reservoir.h"
#include <iostream>

int main() {
    Reservoir res;
    River river1("../../reservoir_test_data/river1.txt");
    River river2("../../reservoir_test_data/river2.txt");
    River river3("../../reservoir_test_data/river3.txt");
    River river4("../../reservoir_test_data/river4.txt");
    res.add_river_in(&river1);
    res.add_river_in(&river2);
    res.add_river_out(&river3);
    res.add_river_out(&river4);
    input_output_var values;
    for (int i = 0; i < 5; i++) {
        values = res.get_next_rivervars();
        std::cout << values.flow_in << "," << values.flow_out << ","
                  << values.load_ammonia_in << "," << values.load_ammonia_out
                  << "," << values.load_nitrate_in << ","
                  << values.load_nitrate_out << "," << values.load_organic_in
                  << "," << values.load_organic_out << "," << std::endl;
    }
}