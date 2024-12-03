import csv
from datetime import datetime

file_name = "arithmetic_operations_log.csv"


with open(file_name, mode='a', newline='') as file:
    writer = csv.writer(file)
    if file.tell() == 0:  
        writer.writerow(["Date & Time", "Operand 1", "Operand 2", "Operation", "Result", "Error", "Execution Status"])

while True:
    try:
        def user_inp():
            global operand2,operand1 
            operand1 = float(input("Enter the first operand: "))
            operand2 = float(input("Enter the second operand: "))
        operation = input("1.Addition\n2.Subtraction\n3.Multiplication\n4.Division\n5.Modulus\nEnter the operation number : ")

        if operation == "1":
            user_inp()
            result = operand1 + operand2
        elif operation == "2":
            user_inp()
            result = operand1 - operand2
        elif operation == "3":
            user_inp()
            result = operand1 * operand2
        elif operation == "4":
            user_inp()
            result = operand1 / operand2
        elif operation == "5":
            user_inp()
            result=operand1 % operand2
        else:
            raise ValueError("Invalid operation.")
        
        print("The result is:", result)
        error = "No Error"
        execution_status = "Success"
    except Exception as e:
        result = None
        error = str(e)
        execution_status = "Failed"
        print("Error occurred:", error)
    else:
        print("No error.")
    finally:
        print("Program executed successfully.")
        with open(file_name, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([datetime.now(), operand1, operand2, operation, result, error, execution_status])
