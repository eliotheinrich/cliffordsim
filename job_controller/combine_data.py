import argparse
import re
import os

from dataframe import DataFrame, load_data


def combine_data(job_name, dir, num_checkpoints=None):
    if num_checkpoints is None:
        pattern = f"{job_name}_(\\d+)(?:_(\\d+))?\\.(json|eve)"
    else:
        pattern = f"{job_name}_(\\d+)(?:_{num_checkpoints})?\\.(json|eve)"

    data = DataFrame()
    total_time = []
    num_jobs = []
    num_threads = []
    for file in os.listdir(dir):
        m = re.search(pattern, file)
        if m is not None:
            print(f"{file} matches pattern")
            _data = load_data(os.path.join(dir, file))
            total_time.append(_data.metadata["total_time"])
            num_jobs.append(_data.metadata["num_jobs"])
            num_threads.append(_data.metadata["num_threads"])

            data += _data
            data.reduce()

    data.add_metadata('total_time', max(total_time))
    data.add_metadata('num_jobs', sum(num_jobs))
    data.add_metadata('num_threads', sum(num_threads))

    return data


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("job_name", type=str, help="Name of the job.")
    parser.add_argument("directory", type=str, help="Case directory.")
    parser.add_argument("extension", type=str, help="Data file extension.")
    parser.add_argument("num_checkpoints", type=str, help="Number of checkpoints so that only final checkpoint file is loaded.")
    args = parser.parse_args()

    job_name = args.job_name
    dir = args.directory
    ext = args.extension
    num_checkpoints = args.num_checkpoints

    data = combine_data(job_name, dir, num_checkpoints)
    data_filename = os.path.join(dir, f'{job_name}.{ext}')
    data.write(data_filename)
