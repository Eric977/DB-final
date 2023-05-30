import subprocess


# Run the YCSB benchmark and get the output
ycsb_command = ["./YCSB/bin/ycsb", "run", "basic", "-P", "./workloads/workloada"]
output = subprocess.run(ycsb_command, capture_output=True, text=True, check=True).stdout


file = "workloads/sample"
f = open(file, "w+")
# Parse the output and run operations on the B+ Tree
for line in output.split("\n"):
    words = line.split()
    # print(words) 

    if len(words) >= 2:
        operation, key = words[0], words[2][4:]  # get operation and key, note that the key starts from 4th index as per YCSB output
        # value = words[3] if len(words) > 3 else None  # get value if it exists
        if operation == "INSERT":
            value = words[4][8:] 
            if words[5] != "]":
                value += words[5]

            # print(key, value)
            f.write(key + " " + value + "\n")
            # print(len(value))
        # Find the corresponding B+ Tree operation
        # btree_operation = operation_mapping.get(operation)


