import sys

if __name__ == "__main__":
    result = []
    actual = []
    with open("result.txt", "r") as f:
        for line in f.readlines():
            result.append(line.split(" ")[0])
    with open("data/test_lbl.txt", "r") as f:
        for line in f.readlines():
            actual.append(line.strip())
    
    correct = 0
    for i in range(len(result)):
        if result[i] == actual[i]:
            correct += 1
    
    precision = correct / len(result)
    print (precision)