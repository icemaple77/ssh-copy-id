#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s user@machine [ssh arguments]\n", argv[0]);
        return 1;
    }

    char publicKey[256];
    snprintf(publicKey, sizeof(publicKey), "%s/.ssh/id_rsa.pub", getenv("USERPROFILE"));

    FILE *publicKeyFile = fopen(publicKey, "r");
    if (!publicKeyFile)
    {
        printf("ERROR: failed to open ID file '%s': No such file\n", publicKey);
        return 1;
    }

    char sshCommand[512];
    snprintf(sshCommand, sizeof(sshCommand), "ssh %s \"umask 077; test -d .ssh || mkdir .ssh ; cat >> .ssh/authorized_keys || exit 1\"", argv[2]);

    FILE *sshProcess = popen(sshCommand, "w");
    if (!sshProcess)
    {
        printf("Failed to start ssh process\n");
        fclose(publicKeyFile);
        return 1;
    }

    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), publicKeyFile)) > 0)
    {
        fwrite(buffer, 1, bytesRead, sshProcess);
    }

    fclose(publicKeyFile);
    pclose(sshProcess);

    printf("Successfully copied public key to %s\n", argv[1]);

    return 0;
}
