#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

#include <string>
#include <vector>
#include "Model.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Model {
  public:
        Model(std::string driver_file);
        Eigen::MatrixXd get_vertices();
        void save_vertices(Eigen::MatrixXd new_vs);
        void save_model(std::ostream &output);
        bool model_loaded();

  private:
        void load_model();
        void add_vertice(double vx,double vy,double vz);
        void output_formatted_float(std::ostream &o, double number, int precision);

        Eigen::MatrixXd Vertices = Eigen::MatrixXd();
        std::string original_file;
        bool load_successful = true;
};
#endif
