import sys
import os
sys.path.insert(1, os.path.join(sys.path[0], '..'))

from dataframe import *
from job_controller import submit_jobs
import numpy as np

def generate_config(system_size, h, state_type=0, delta=1.0, sre_type="montecarlo", num_runs=1):
    config = {}
    config["config_type"] = "quantum_ising"
    config["num_runs"] = num_runs

    system_size = list(system_size)
    zparams = [{"system_size": L, "magic_mutual_information_subsystem_size": L//8} for L in system_size]
    config["h"] = h
    config["bond_dimension"] = 50
    config["num_sweeps"] = 500

    config["state_type"] = state_type
    config["delta"] = delta

    config["sample_probabilities"] = False
    config["sample_bitstring_distribution"] = False

    config["stabilizer_renyi_indices"] = "2"
    config["sre_mc_equilibration_timesteps"] = 5000
    config["sample_stabilizer_renyi_entropy"] = False
    config["sre_method"] = sre_type
    config["sre_num_samples"] = 50000
    config["sample_magic_mutual_information"] = False
    config["sample_bipartite_magic_mutual_information"] = True
    config["zparams"] = zparams

    return config


if __name__ == "__main__":
    L = [8]
    h = np.linspace(0.0, 2.0, 50)
    for Li in L:
        param_matrix = generate_config([Li], h, state_type=[0], sre_type=["exhaustive", "montecarlo", "virtual"], num_runs=1)
        submit_jobs(f"ising_test{Li}", param_bundle=param_matrix, ncores=4, memory="10gb", time="6:00:00", nodes=1, cleanup=False, run_local=True)

    param_matrix = generate_config(L, h, sample_magic=True, sre_type=["virtual"], num_runs=1)
    #submit_jobs(f"xxz_test", param_bundle=param_matrix, ncores=64, memory="10gb", time="6:00:00", nodes=1, cleanup=False)

    L = [256]
    h = 1.0
    param_matrix = generate_config(L, h, sample_magic=False, num_runs=1)
    #submit_jobs(f"ising_entanglement_test", param_bundle=param_matrix, ncores=4, memory="10gb", time="6:00:00", nodes=1, cleanup=False, run_local=True)
