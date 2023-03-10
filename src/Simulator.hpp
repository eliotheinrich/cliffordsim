#ifndef SIM_H
#define SIM_H

#include <iostream>

#include <DataFrame.hpp>
#include <algorithm>
#include <numeric>

template <typename T>
static void print_vector(std::vector<T> v) {
    std::cout << "[";
    for (auto const &t : v) {
        std::cout << t << ", ";
    } std::cout << "]\n";
}

class Simulator {
    public:
        virtual void timesteps(uint num_steps)=0;
};

class TimeConfig : public Config, public Simulator {
    private:
        uint nruns;

        virtual void init_state()=0;
        virtual std::map<std::string, Sample> take_samples()=0;

    public:
        uint sampling_timesteps;
        uint equilibration_steps;
        uint measurement_freq;
        bool temporal_avg;

        TimeConfig(std::map<std::string, int> iparams, std::map<std::string, float> fparams) {
            sampling_timesteps = iparams["sampling_timesteps"];
            equilibration_steps = iparams["equilibration_timesteps"];
            measurement_freq = iparams["measurement_freq"];
            temporal_avg = (bool) iparams["temporal_avg"];
        }

        virtual uint get_nruns() const { return nruns; }

        void compute(DataSlide *slide) {
            init_state();

            timesteps(equilibration_steps);

            int num_timesteps, num_intervals;
            if (sampling_timesteps == 0) {
                num_timesteps = 0;
                num_intervals = 1;
            } else {
                num_timesteps = measurement_freq;
                num_intervals = sampling_timesteps/measurement_freq;
            }


            std::map<std::string, std::vector<Sample>> samples;
            std::map<std::string, Sample> sample = take_samples();
            std::vector<std::string> keys(0);
            for (auto const& [key, val] : sample) {
                keys.push_back(key);
            }

            for (int t = 0; t < num_intervals; t++) {
                timesteps(num_timesteps);
                sample = take_samples();
                for (auto key : keys)
                    samples[key].push_back(sample[key]);
            }

            for (auto const &[key, ksamples] : samples) {
                slide->add_data(key);
                if (temporal_avg) {
                    slide->push_data(key, Sample::collapse(ksamples));
                } else {
                    for (auto s : ksamples) {
                        slide->push_data(key, s);
                    }
                }
            }
        }
};

class Entropy {
    public:
        uint system_size;
        uint partition_size;
        uint spacing;

        Entropy() {}

        Entropy(std::map<std::string, int> iparams, std::map<std::string, float> fparams) {
            system_size = iparams["system_size"];
            partition_size = iparams["partition_size"];
            spacing = iparams["spacing"];
        }

        virtual float entropy(std::vector<uint> &sites) const=0;

        Sample spatially_averaged_entropy(uint system_size, uint partition_size, uint spacing) const {
            std::vector<uint> sites(partition_size);
            std::iota(sites.begin(), sites.end(), 0);

            uint num_partitions = std::max((system_size - partition_size)/spacing, 1u);

            float s = 0.;
            float s2 = 0.;

            std::vector<uint> offset_sites(partition_size);
            for (uint i = 0; i < num_partitions; i++) { // TODO test
                std::transform(sites.begin(), 
                               sites.end(),
                               offset_sites.begin(), 
                               [i, spacing, system_size](uint x) { return (x + i*spacing) % system_size; });

                float st = entropy(offset_sites);
                s += st;
                s2 += st*st;
            }

            s /= num_partitions;
            s2 /= num_partitions;

            float stdev = std::sqrt(std::abs(s2 - s*s));

            return Sample(s, stdev, num_partitions);        
        }

        Sample spatially_averaged_entropy() const {
            return spatially_averaged_entropy(system_size, partition_size, spacing);
        }

};

#endif