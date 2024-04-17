#include "../include/Reservoir.h"
#include <iostream>

int main() {
    Reservoir res;
    River river1("../../reservoir_test_data/river1.txt");
    River river2("../../reservoir_test_data/river2.txt");
    River river3("../../reservoir_test_data/river3.txt");
    River river4("../../reservoir_test_data/river4.txt");
    Nitrification nitri;
    Denitrification deni;
    res.init(130, 0, 0, 0, nitri, deni);
    res.add_river_in(&river1);
    res.add_river_in(&river2);
    res.add_river_out(&river3);
    res.add_river_out(&river4);

    std::cout<<res.current_status().wl<<std::endl;
    input_output_var values;
    for (int i = 0; i < 5; i++) {
        values = res.get_next_rivervars();
        std::cout << values.flow_in << "," << values.flow_out << ","
                  << values.load_ammonia_in << "," << values.load_ammonia_out
                  << "," << values.load_nitrate_in << ","
                  << values.load_nitrate_out << "," << values.load_organic_in
                  << "," << values.load_organic_out << "," << std::endl;
        res.predict(1);
        std::cout<<res.current_status().wl<<std::endl;
    }
    // 水量转水位测试，需要将方法转化为public
    // double volumn = res.wl_to_volumn(136.33);
    // std::cout<<volumn<<std::endl;
    // double wl = res.volumn_to_wl(volumn);
    // std::cout<<wl<<std::endl;
}