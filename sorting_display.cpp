void sortLeaderboard(Team teams[], int size)
{
    (void)teams; (void)size;
    /* TODO: descending score; swap complete Team objects */
    for(int i=0; i<size-1; i++){
        int maxIndex=i;
        for(int j=i+1; j<size; j++){
            if(teams[maxIndex].score<teams[j].score){
                maxIndex=j;
            }else if (teams[maxIndex].score==teams[j].score && 
                teams[maxIndex].missions>teams[j].missions){
                maxIndex=j;
            }
        }
        if(maxIndex!=i){
            Team temp=teams[maxIndex];
            teams[maxIndex]=teams[i];
            teams[i]=temp;
        }
    }
}

void displayTeams(const Team teams[], int size)
{
    (void)teams; (void)size;
    /* TODO: print a readable table without inspecting unused capacity */
    printf("+--------+------------------------------------------+----------+----------+\n");
    printf("| %-6s | %-40s | %-8s | %-8s |\n", "ID", "Name", "Score", "Missions");
    printf("+--------+------------------------------------------+----------+----------+\n");

    for (int i=0; i<size; i++) {
        printf("| %-6d | %-40s | %-8d | %-8d |\n", 
               teams[i].id, 
               teams[i].name, 
               teams[i].score, 
               teams[i].missions);
    }

    printf("+--------+------------------------------------------+----------+----------+\n");
    cout<<"Total: "<<size<<" teams"<<endl;
}