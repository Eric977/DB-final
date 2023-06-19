import subprocess
import argparse





if __name__ == '__main__':
    # Run the YCSB benchmark and get the output

    parser = argparse.ArgumentParser()
    parser.add_argument('-w', '--workload', type=str, required='workloads/workloadb', help='workload config file')
    parser.add_argument('-o', '--output', type=str, default='workloads/sample', help='output file')


    args = parser.parse_args()
    ycsb_command = ["./third_party/YCSB/bin/ycsb.sh", "run", "basic", "-P", args.workload]
    output = subprocess.run(ycsb_command, capture_output=True, text=True, check=True).stdout

    file = args.output
    f = open(file, "w+")
    # Parse the output and run operations on the B+ Tree
    s = set()
    for line in output.split("\n"):
        words = line.split()


        if len(words) >= 2:
            operation, key = words[0], words[2][:]  # get operation and key, note that the key starts from 4th index as per YCSB output
            # value = words[3] if len(words) > 3 else None  # get value if it exists
            if operation == "INSERT":
                value = words[4][8:] 
                if words[5] != "]":
                    value += words[5]

                # print(key, value)
                f.write(key + " " + value + "\n")
                # print(len(value))
            else:
                s.add(key)
                f.write(key + "\n")
                
            # Find the corresponding B+ Tree operation
            # btree_operation = operation_mapping.get(operation)

    print(len(s))

