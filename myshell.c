#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>

int argc;              // 参数个数
char *argv[10];        // 参数
char command[10][100]; // 参数
char buf[100];         // 接受输入的参数
char backupBuf[100];   // 参数备份
char curPath[100];     // 工作路
int i, j;              // 循环

int get_input(char buf[]); // 输入指令并存入buf数组
void separate(char *buf);  // 分解字符串
void do_cmd(int argc, char *argv[]);
int callCd(int argc); // cd

int cmdoutput(char buf[100]);  // 执行输出重定向
int cmdinput(char buf[100]);   // 执行输入重定向命令
int cmdRoutput(char buf[100]); // 执行输出重定向追加写
int cmdpipe(char buf[100]);    // 执行管道命令
int cmdbg(char buf[100]);

int get_input(char buf[])
{

    memset(buf, 0, 100);
    memset(backupBuf, 0, 100);

    fgets(buf, 100, stdin);

    buf[strlen(buf) - 1] = '\0';
    return strlen(buf);
}

void separate(char *buf)
{

    for (i = 0; i < 10; i++)
    {
        argv[i] = NULL;
        for (j = 0; j < 100; j++)
            command[i][j] = '\0';
    }
    argc = 0;

    strcpy(backupBuf, buf);

    int len = strlen(buf);
    for (i = 0, j = 0; i < len; ++i)
    {
        if (buf[i] != ' ')
        {
            command[argc][j++] = buf[i];
        }
        else
        {
            if (j != 0)
            {
                command[argc][j] = '\0';
                ++argc;
                j = 0;
            }
        }
    }
    if (j != 0)
    {
        command[argc][j] = '\0';
    }

    argc = 0;
    int flg = 0;
    for (i = 0; buf[i] != '\0'; i++)
    {
        if (flg == 0 && !isspace(buf[i]))
        {
            flg = 1;
            argv[argc++] = buf + i;
        }
        else if (flg == 1 && isspace(buf[i]))
        {
            flg = 0;
            buf[i] = '\0';
        }
    }
    argv[argc] = NULL;
}

void do_cmd(int argc, char *argv[])
{
    pid_t pid;

    for (j = 0; j < 10; j++)
    {
        if (strcmp(command[j], ">") == 0)
        {
            strcpy(buf, backupBuf);
            int sample = cmdoutput(buf);
            return;
        }
    }

    for (j = 0; j < 10; j++)
    {
        if (strcmp(command[i], "<") == 0)
        {
            strcpy(buf, backupBuf);
            int sample = cmdinput(buf);
            return;
        }
    }

    for (j = 0; j < 10; j++)
    {
        if (strcmp(command[j], ">>") == 0)
        {
            strcpy(buf, backupBuf);
            int sample = cmdRoutput(buf);
            return;
        }
    }

    for (j = 0; j < 10; j++)
    {
        if (strcmp(command[j], "|") == 0)
        {
            strcpy(buf, backupBuf);
            int sample = cmdpipe(buf);
            return;
        }
    }

    for (j = 0; j < 10; j++)
    {
        if (strcmp(command[j], "&") == 0)
        {
            strcpy(buf, backupBuf);
            int sample = cmdbg(buf);
            return;
        }
    }

    if (strcmp(command[0], "cd") == 0)
    {
        int res = callCd(argc);
        if (!res)
            printf("指令错误");
    }
    else if (strcmp(command[0], "exit") == 0)
    {
        exit(0);
    }
    else
    {
        switch (pid = fork())
        {

        case -1:

            return;

        case 0:
        {
            execvp(argv[0], argv);

            printf("%s: 命令输入错误\n", argv[0]);

            exit(1);
        }
        default:
        {
            int status;
            waitpid(pid, &status, 0);
        }
        }
    }
}

int callCd(int argc)
{

    int result = 1;
    if (argc != 2)
    {
        printf("指令错误");
    }
    else
    {
        int ret = chdir(command[1]);
        if (ret)
            return 0;
    }

    if (result)
    {
        char *res = getcwd(curPath, 100);
        if (res == NULL)
        {
            printf("文件路径不存在");
        }
        return result;
    }
    return 0;
}

int cmdoutput(char buf[100])
{
    strcpy(buf, backupBuf);
    char outFile[100];
    memset(outFile, 0x00, 100);
    int RediNum = 0;
    for (i = 0; i + 1 < strlen(buf); i++)
    {
        if (buf[i] == '>' && buf[i + 1] == ' ')
        {
            RediNum++;
            break;
        }
    }
    if (RediNum != 1)
    {
        return 0;
    }

    for (i = 0; i < argc; i++)
    {
        if (strcmp(command[i], ">") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(outFile, command[i + 1]);
            }
            else
            {
                return 0;
            }
        }
    }

    for (j = 0; j < strlen(buf); j++)
    {
        if (buf[j] == '>')
        {
            break;
        }
    }
    buf[j - 1] = '\0';
    buf[j] = '\0';

    separate(buf);
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
    {
        return 0;
    }

    case 0:
    {

        int fd;
        fd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        if (fd < 0)
        {
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        execvp(argv[0], argv);
        if (fd != STDOUT_FILENO)
        {
            close(fd);
        }

        printf("%s: 命令输入错误\n", argv[0]);

        exit(1);
    }
    default:
    {
        int status;
        waitpid(pid, &status, 0);
    }
    }
}

int cmdinput(char buf[100])
{
    strcpy(buf, backupBuf);
    char inFile[100];
    memset(inFile, 0x00, 100);
    int RediNum = 0;
    for (i = 0; i + 1 < strlen(buf); i++)
    {
        if (buf[i] == '<' && buf[i + 1] == ' ')
        {
            RediNum++;
            break;
        }
    }
    if (RediNum != 1)
    {
        return 0;
    }

    for (i = 0; i < argc; i++)
    {
        if (strcmp(command[i], "<") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(inFile, command[i + 1]);
            }
            else
            {
                return 0;
            }
        }
    }

    for (j = 0; j < strlen(buf); j++)
    {
        if (buf[j] == '<')
        {
            break;
        }
    }
    buf[j - 1] = '\0';
    buf[j] = '\0';
    separate(buf);
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
    {
        return 0;
    }

    case 0:
    {

        int fd;
        fd = open(inFile, O_RDWR);

        if (fd < 0)
        {
            exit(1);
        }
        dup2(fd, STDIN_FILENO);
        execvp(argv[0], argv);
        if (fd != STDIN_FILENO)
        {
            close(fd);
        }

        printf("%s: 命令错误\n", argv[0]);

        exit(1);
    }
    default:
    {
        int status;
        waitpid(pid, &status, 0);
    }
    }
}

int cmdRoutput(char buf[100])
{
    strcpy(buf, backupBuf);
    char reOutFile[100];
    memset(reOutFile, 0x00, 100);
    int RediNum = 0;
    for (i = 0; i + 2 < strlen(buf); i++)
    {
        if (buf[i] == '>' && buf[i + 1] == '>' && buf[i + 2] == ' ')
        {
            RediNum++;
            break;
        }
    }
    if (RediNum != 1)
    {
        return 0;
    }

    for (i = 0; i < argc; i++)
    {
        if (strcmp(command[i], ">>") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(reOutFile, command[i + 1]);
            }
            else
            {
                return 0;
            }
        }
    }

    for (j = 0; j + 2 < strlen(buf); j++)
    {
        if (buf[j] == '>' && buf[j + 1] == '>' && buf[j + 2] == ' ')
        {
            break;
        }
    }
    buf[j - 1] = '\0';
    buf[j] = '\0';

    separate(buf);
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
    {
        return 0;
    }

    case 0:
    {

        int fd;
        fd = open(reOutFile, O_WRONLY | O_CREAT, 0777);

        if (fd < 0)
        {
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        execvp(argv[0], argv);
        if (fd != STDOUT_FILENO)
        {
            close(fd);
        }

        printf("%s: 命令错误\n", argv[0]);

        exit(1);
    }
    default:
    {
        int status;
        waitpid(pid, &status, 0);
    }
    }
}

int cmdpipe(char buf[100])
{

    for (j = 0; buf[j] != '\0'; j++)
    {
        if (buf[j] == '|')
            break;
    }

    char outputBuf[j];
    memset(outputBuf, 0x00, j);
    char inputBuf[strlen(buf) - j];
    memset(inputBuf, 0x00, strlen(buf) - j);
    for (i = 0; i < j - 1; i++)
    {
        outputBuf[i] = buf[i];
    }
    for (i = 0; i < strlen(buf) - j - 1; i++)
    {
        inputBuf[i] = buf[j + 2 + i];
    }

    int pd[2];
    pid_t pid;
    if (pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    if (pid == 0)
    {
        close(pd[0]);
        dup2(pd[1], STDOUT_FILENO);
        separate(outputBuf);
        execvp(argv[0], argv);
        if (pd[1] != STDOUT_FILENO)
        {
            close(pd[1]);
        }
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);

        close(pd[1]);
        dup2(pd[0], STDIN_FILENO);
        separate(inputBuf);
        execvp(argv[0], argv);
        if (pd[0] != STDIN_FILENO)
        {
            close(pd[0]);
        }
    }

    return 1;
}

int cmdbg(char buf[100])
{
    char backgroundBuf[strlen(buf)];
    memset(backgroundBuf, 0x00, strlen(buf));
    for (i = 0; i < strlen(buf); i++)
    {
        backgroundBuf[i] = buf[i];
        if (buf[i] == '&')
        {
            backgroundBuf[i] = '\0';
            backgroundBuf[i - 1] = '\0';
            break;
        }
    }

    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    if (pid == 0)
    {

        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "r", stdin);
        signal(SIGCHLD, SIG_IGN);
        separate(backgroundBuf);
        execvp(argv[0], argv);
        printf("%s: 命令错误\n", argv[0]);
        exit(1);
    }
    else
    {
        exit(0);
    }
}

int main()
{

    while (1)
    {
        printf("myshell$ ");
        if (get_input(buf) == 0)
            continue;
        strcpy(backupBuf, buf);
        separate(buf);
        do_cmd(argc, argv);
        argc = 0;
    }
}