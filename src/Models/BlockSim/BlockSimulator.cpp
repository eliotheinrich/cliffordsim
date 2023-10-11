#include "BlockSimulator.h"

#define DEFAULT_RANDOM_SITE_SELECTION true
#define DEFAULT_PRECUT false

#define DEFAULT_SAMPLE_AVALANCHE_SIZES true

#define DEFAULT_DEPOSITING_TYPE 0

//  (1)  |  (2)  |  (3)  |  (4)  |  (5)  |  (6)  |
//       |       |       |       |       | o     |
//       |   o   | o   o | o     | o o   | o o   |
// o o o | o o o | o o o | o o o | o o o | o o o |

BlockSimulator::BlockSimulator(Params &params) : Simulator(params), start_sampling(false) {
    system_size = get<int>(params, "system_size");
    pm = get<double>(params, "pm");
    pu = get<double>(params, "pu");

    precut = get<int>(params, "precut", DEFAULT_PRECUT);

    random_sites = get<int>(params, "random_sites", DEFAULT_RANDOM_SITE_SELECTION);

    feedback_mode = get<int>(params, "feedback_mode");
	depositing_type = get<int>(params, "depositing_type", DEFAULT_DEPOSITING_TYPE);

	if      (feedback_mode == 0)  feedback_strategy = std::vector<uint32_t>{1};
	else if (feedback_mode == 1)  feedback_strategy = std::vector<uint32_t>{1, 2};
	else if (feedback_mode == 2)  feedback_strategy = std::vector<uint32_t>{1, 3};
	else if (feedback_mode == 3)  feedback_strategy = std::vector<uint32_t>{1, 4};
	else if (feedback_mode == 4)  feedback_strategy = std::vector<uint32_t>{1, 5};
	else if (feedback_mode == 5)  feedback_strategy = std::vector<uint32_t>{1, 6};
	else if (feedback_mode == 6)  feedback_strategy = std::vector<uint32_t>{1, 2, 3};
	else if (feedback_mode == 7)  feedback_strategy = std::vector<uint32_t>{1, 2, 4};
	else if (feedback_mode == 8)  feedback_strategy = std::vector<uint32_t>{1, 2, 5};
	else if (feedback_mode == 9)  feedback_strategy = std::vector<uint32_t>{1, 2, 6};
	else if (feedback_mode == 10) feedback_strategy = std::vector<uint32_t>{1, 3, 4};
	else if (feedback_mode == 11) feedback_strategy = std::vector<uint32_t>{1, 3, 5};
	else if (feedback_mode == 12) feedback_strategy = std::vector<uint32_t>{1, 3, 6};
	else if (feedback_mode == 13) feedback_strategy = std::vector<uint32_t>{1, 4, 5};
	else if (feedback_mode == 14) feedback_strategy = std::vector<uint32_t>{1, 4, 6};
	else if (feedback_mode == 15) feedback_strategy = std::vector<uint32_t>{1, 5, 6};
	else if (feedback_mode == 16) feedback_strategy = std::vector<uint32_t>{1, 2, 3, 4};
	else if (feedback_mode == 17) feedback_strategy = std::vector<uint32_t>{1, 2, 3, 5};
	else if (feedback_mode == 18) feedback_strategy = std::vector<uint32_t>{1, 2, 3, 6};
	else if (feedback_mode == 19) feedback_strategy = std::vector<uint32_t>{1, 2, 4, 5};
	else if (feedback_mode == 20) feedback_strategy = std::vector<uint32_t>{1, 2, 4, 6};
	else if (feedback_mode == 21) feedback_strategy = std::vector<uint32_t>{1, 2, 5, 6};
	else if (feedback_mode == 22) feedback_strategy = std::vector<uint32_t>{1, 3, 4, 5};
	else if (feedback_mode == 23) feedback_strategy = std::vector<uint32_t>{1, 3, 4, 6};
	else if (feedback_mode == 24) feedback_strategy = std::vector<uint32_t>{1, 3, 5, 6};
	else if (feedback_mode == 25) feedback_strategy = std::vector<uint32_t>{1, 4, 5, 6};
	else if (feedback_mode == 26) feedback_strategy = std::vector<uint32_t>{1, 2, 3, 4, 5};
	else if (feedback_mode == 27) feedback_strategy = std::vector<uint32_t>{1, 2, 3, 4, 6};
	else if (feedback_mode == 28) feedback_strategy = std::vector<uint32_t>{1, 2, 3, 5, 6};
	else if (feedback_mode == 29) feedback_strategy = std::vector<uint32_t>{1, 2, 4, 5, 6};
	else if (feedback_mode == 30) feedback_strategy = std::vector<uint32_t>{1, 3, 4, 5, 6};

	sample_avalanche_sizes = get<int>(params, "sample_avalanche_sizes", DEFAULT_SAMPLE_AVALANCHE_SIZES);
	avalanche_sizes = std::vector<uint32_t>(system_size, 0);
}

std::string BlockSimulator::to_string() const {
    std::string s = "[";
    std::vector<std::string> buffer;
    for (uint32_t i = 0; i < system_size; i++) buffer.push_back(std::to_string(surface[i]));
    s += join(buffer, ", ") + "]";

    return s;
}

uint32_t BlockSimulator::get_shape(uint32_t s0, uint32_t s1, uint32_t s2) const {
	int ds1 = s0 - s1;
	int ds2 = s2 - s1;

	if      ((ds1 == 0)  && (ds2 == 0))   return 1; // 0 0 0 (a)
	else if ((ds1 == -1) && (ds2 == -1))  return 2; // 0 1 0 (b)
	else if ((ds1 == 1)  && (ds2 == 1))   return 3; // 1 0 1 (c)
	else if ((ds1 == 0)  && (ds2 == 1))   return 4; // 0 0 1 (d)
	else if ((ds1 == 1)  && (ds2 == 0))   return 4; // 1 0 0
	else if ((ds1 == 0)  && (ds2 == -1))  return 5; // 1 1 0 (e)
	else if ((ds1 == -1) && (ds2 == 0))   return 5; // 0 1 1
	else if ((ds1 == -1) && (ds2 == 1))   return 6; // 0 1 2 (f)
	else if ((ds1 == 1)  && (ds2 == -1))  return 6; // 2 1 0
	else {
std::cout << s0 << " " << s1 << " " << s2 << std::endl;
std::cout << "ds1 = " << ds1 << ", " << "ds2 = " << ds2 << std::endl;
		throw std::invalid_argument("Something has gone wrong with the entropy substrate.");
	}
}

void BlockSimulator::avalanche(uint32_t i) {
    if (precut && surface[i] > 0) surface[i]--;

    uint32_t left = i;
    while (surface[left-1] > surface[i]) left--;

    uint32_t right = i;
    while (surface[right+1] > surface[i]) right++;

	for (uint32_t j = left; j < i; j++) surface[j]--;
	for (uint32_t j = i+1; j < right+1; j++) surface[j]--;
    
    uint32_t size = right - left;
    if (size > 0)
        record_size(size);
}

bool BlockSimulator::can_deposit(uint32_t i) const {
	if (i == 0 || i == system_size - 1)
		return false;

	uint32_t shape = get_shape(surface[i-1], surface[i], surface[i+1]);

	bool b = shape == 1 || shape == 3 || shape == 4;

	//             (a)           (c)           (d)
	return b;
}

void BlockSimulator::deposit(uint32_t i) {
	if (depositing_type == 0) {
		if (can_deposit(i))
			surface[i]++;
	} else if (depositing_type == 1) {
		bool continue_depositing = true;

		while (continue_depositing) {
			continue_depositing = false;
			if (can_deposit(i-1)) {
				surface[i-1]++;
				continue_depositing = true;
			}
			
			if (can_deposit(i)) {
				surface[i]++;
				continue_depositing = true;
			}

			if (can_deposit(i+1)) {
				surface[i+1]++;
				continue_depositing = true;
			}
		}
	}
}

void BlockSimulator::timesteps(uint32_t num_steps) {
    for (uint32_t i = 0; i < num_steps; i++) {
        for (uint32_t i = 1; i < system_size - 1; i++) {
            uint32_t q = random_sites ? rand() % (system_size - 2) + 1 : i;
            
            uint32_t shape = get_shape(surface[q-1], surface[q], surface[q+1]);

            if (std::count(feedback_strategy.begin(), feedback_strategy.end(), shape)) {
                if (randf() < pu)
					deposit(q);
            } else {
                if (randf() < pm)
					avalanche(q);
            }
        }
    }
}

void BlockSimulator::add_avalanche_samples(data_t &samples) {
	uint32_t total_avalanches = 0;
	for (uint32_t i = 0; i < system_size; i++)
		total_avalanches += avalanche_sizes[i];

	if (total_avalanches == 0) {
		for (uint32_t i = 0; i < system_size; i++) 
			samples.emplace("avalanche_" + std::to_string(i), 0.0);
	} else {
		for (uint32_t i = 0; i < system_size; i++) 
			samples.emplace("avalanche_" + std::to_string(i), double(avalanche_sizes[i])/total_avalanches);
	}

    avalanche_sizes = std::vector<uint32_t>(system_size, 0);
}

data_t BlockSimulator::take_samples() {
    data_t samples;
    for (uint32_t i = 0; i < system_size; i++) samples.emplace("surface_" + std::to_string(i), surface[i]);

	if (sample_avalanche_sizes)
		add_avalanche_samples(samples);

    return samples;
}

