#include "../reactor/include/Reservoir.h"
#include<iostream>

int main(){
    Reservoir res("./output/res_status.txt");
    River river_bai("./tmp/baihe.txt");
    River river_chao("./tmp/chaohe.txt");
    River river_ns("./tmp/ns.txt");
    River river_baidam("./tmp/baidam.txt");
    River river_chaodam("./tmp/chaodam.txt");

    // 河系配置
    res.add_river_in(&river_bai);
    res.add_river_in(&river_chao);
    res.add_river_in(&river_ns);
    res.add_river_out(&river_baidam);
    res.add_river_out(&river_chaodam);

    // 硝化系统配置
    Nitrification nitri;
    nitri.init(0, 0, 0, 0, 0, 0, 0, 0);

    //反硝化系统配置
    Denitrification denitri;
    denitri.init(0, 0, 0, 0, 0, 0, 1);

    //水库初始化
    res.init(133.19, 0, 0, 0, nitri, denitri);

    int timestep = 3023, dt = 1;
    for(int i=0; i < timestep; i++){
        res.predict(dt);
        res.get_next_rivervars();
        res.write();
    }
}