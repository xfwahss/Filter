#include <ReservoirSystem.h>
#include <ExcelIO.h>
#include <logger.h>

void simulate(const std::string &filename_in, const std::string &filename_out){
    ExcelIO file_in(filename_in, "r");
    ExcelIO file_out(filename_out, "w");

    // 这里需要读取一个初始状态
    Eigen::VectorXd status;
    int steps = 100;
    int in_nums = 2;
    int out_nums = 2;
    double dt = 1;
    for(int i=0; i < steps; ++i){
        logger::get("1DReservoirModel")->info("Index: {}", i+1);
        Eigen::VectorXd bnd_force = file_in.read_row("BndForce", i, 1, 33);
        status = ReservoirSystem::predict(status, bnd_force, in_nums, out_nums, dt);
        file_out.write_row(status, "Simulation", i, 1);
    }
}
int main(int argc, char* argv[]){
    simulate("This", "That");
}