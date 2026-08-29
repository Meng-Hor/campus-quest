int loadTeams(const char *filename, Team **teams, int *size, int *capacity){

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return 0;
    }
    char line[200];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        Team candidate;

        // scanf 1 field to another
        int fields = sscanf(line, "%d|%39[^|]|%d|%d", &candidate.id, candidate.name, &candidate.score, &candidate.missions);

        // Reject malformed lines
        if (fields != 4)
        {
            continue;
        }
        addTeam(teams, size, capacity, candidate);
    }

    fclose(file);

    return 1;
}

int findTeamIndex(const Team teams[], int size, int id){
    for (int i=0; i<size; i++){
        if (teams[i].id == id){
            return i;
        }
    }
    return -1;
}