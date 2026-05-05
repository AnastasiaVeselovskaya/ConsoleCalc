#include <integermath/integermath.h>

#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct Task
{
    int64_t first_num;
    int64_t second_num;
    char operation;
    double result;
    int status;
};

void displayHelp()
{
    printf("Usage: ./consolecalc [-h | --help] [-l | --left-number] <number> "
           "[-o | --operation] <operation> [-r | --right-number] <number>\n"
           "   OR: ./consolecalc <left-number> <operation> <right-number>\n"
           "-------------------------------------------------------------------"
           "---------------------------------------------------------------\n"
           "\tNote that only operations below are supported now:\n"
           "\t+ - '/' '*' ! ^\n"
           "\tUnary operations will only use --left-number and do not require "
           "--right-number.\n");
}

bool isValidOperation(char operation)
{
    return operation == '+' || operation == '-' || operation == '*' ||
           operation == '/' || operation == '!' || operation == '^';
}

Task makeTask(int argc, char** argv)
{
    Task taskBody{};

    static struct option int64TOptions[] = {
        {"help", no_argument, nullptr, 'h'},
        {"left-number", required_argument, nullptr, 'l'},
        {"operation", required_argument, nullptr, 'o'},
        {"right-number", required_argument, nullptr, 'r'},
        {nullptr, 0, nullptr, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "hl:o:r:", int64TOptions, nullptr)) != -1)
    {
        switch (opt)
        {
            case 'h':
                displayHelp();
                break;
            case 'l':
                taskBody.first_num = atol(optarg);
                break;
            case 'r':
                taskBody.second_num = atol(optarg);
                break;
            case 'o':
                if (strlen(optarg) == 1 && isValidOperation(optarg[0]))
                {
                    taskBody.operation = optarg[0];
                }
                else
                {
                    taskBody.status = -1;
                }
                break;
            default:
                taskBody.status = -1;
                break;
        }
    }

    int remaining_args = argc - optind;

    if (remaining_args == 3)
    {
        if (taskBody.first_num == 0)
        {
            taskBody.first_num = atol(argv[optind]);
        }

        const char* op_str = argv[optind + 1];
        if (strlen(op_str) == 1 && isValidOperation(op_str[0]))
        {
            taskBody.operation = op_str[0];
        }
        else
        {
            taskBody.status = -1;
        }

        taskBody.second_num = atol(argv[optind + 2]);
    }
    else if (remaining_args == 2)
    {
        const char* opStr = argv[optind + 1];
        if (strlen(opStr) == 1 && opStr[0] == '!')
        {
            taskBody.first_num = atol(argv[optind]);
            taskBody.operation = opStr[0];
        }
    }

    return taskBody;
}

int makeCalculate(Task& task)
{
    switch (task.operation)
    {
        case '+':
            task.status =
                integermath::sum(task.first_num, task.second_num, task.result);
            break;
        case '-':
            task.status = integermath::subtract(task.first_num, task.second_num,
                                                task.result);
            break;
        case '*':
            task.status = integermath::multiply(task.first_num, task.second_num,
                                                task.result);
            break;
        case '/':
            task.status = integermath::divide(task.first_num, task.second_num,
                                              task.result);
            break;
        case '!':
            task.status = integermath::factorial(task.first_num, task.result);
            break;
        case '^':
            task.status = integermath::power(task.first_num, task.second_num,
                                             task.result);
            break;
        default:
            break;
    }
    return 0;
}

void printResult(const Task& task)
{
    if (task.status == -1) {
        printf("Operation resulted in type overflow\n");
        return;
    }

    if (task.result == (int64_t)task.result)
    {
        if (task.operation == '!') {
            printf("%ld%c = %.0f\n", task.first_num, task.operation,
                   task.result);
            return;
        }
        printf("%ld %c %ld = %.0f\n", task.first_num, task.operation,
               task.second_num, task.result);
        return;
    }

    printf("%ld %c %ld = %g\n", task.first_num, task.operation,
            task.second_num, task.result);
}

int applicationRun(int argc, char** argv)
{
    Task newTask = makeTask(argc, argv);
    if (newTask.status == -1 || newTask.operation == '\0')
    {
        displayHelp();
        return -1;
    }
    makeCalculate(newTask);
    printResult(newTask);
    return newTask.status;
}

int main(int argc, char** argv)
{
    applicationRun(argc, argv);
}
