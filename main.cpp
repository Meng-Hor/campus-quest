#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

#define NAME_LEN 40
#define LINE_LEN 160

typedef struct {
    int id;
    char name[NAME_LEN];
    int score;
    int missions;
} Team;

int findTeamIndex(const Team teams[], int size, int id);
int ensureCapacity(Team **teams, int *capacity, int required);
int addTeam(Team **teams, int *size, int *capacity, Team candidate);
int recordMission(Team teams[], int size, int id, int points);
int deleteTeam(Team teams[], int *size, int id);
void sortLeaderboard(Team teams[], int size);
void displayTeams(const Team teams[], int size);
int loadTeams(const char *filename, Team **teams, int *size, int *capacity);
int saveTeams(const char *filename, const Team teams[], int size);
void readText(const char *prompt, char text[], int limit);
int readInt(const char *prompt, int *value);

int main(void)
{
    Team *teams = nullptr;
    int size = 0;
    int capacity = 0;
    int choice;

    loadTeams("teams.txt", &teams, &size, &capacity);

    do {
        cout << "\n=== CAMPUS QUEST LEADERBOARD ===\n"
                  << "1. Register a team\n2. Record mission points\n3. Find a team\n"
                  << "4. Remove a team\n5. Show leaderboard\n6. Save and exit\n";

        if (!readInt("Choose: ", &choice)) {
            cout << "Invalid menu input.\n";
            continue;
        }

        if (choice == 1) {
            Team candidate = {0, "", 0, 0};
            readInt("Team ID: ", &candidate.id);
            readText("Team name: ", candidate.name, NAME_LEN);
            /* TODO: call addTeam and report success/failure */
        } else if (choice == 2) {
            int id, points;
            readInt("Team ID: ", &id);
            readInt("Mission points (1-100): ", &points);
            /* TODO: call recordMission and report success/failure */
        } else if (choice == 3) {
            int id;
            readInt("Team ID: ", &id);
            /* TODO: search and display the matching complete record */
        } else if (choice == 4) {
            int id;
            readInt("Team ID: ", &id);
            /* TODO: call deleteTeam and report success/failure */
        } else if (choice == 5) {
            sortLeaderboard(teams, size);
            displayTeams(teams, size);
        } else if (choice == 6) {
            if (!saveTeams("teams.txt", teams, size))
                cout << "Warning: data could not be saved.\n";
        } else {
            cout << "Choose a number from 1 to 6.\n";
        }
    } while (choice != 6);

    free(teams);
    teams = nullptr;
    return 0;
}

int findTeamIndex(const Team teams[], int size, int id){
    for (int i=0; i<size; i++){
        if (teams[i].id == id){
            return i;
        }
    }
    return -1;
}

int ensureCapacity(Team **teams, int *capacity, int required)
{
    if (required <= *capacity)
        return 1;

    int newCap = (*capacity == 0) ? 1 : *capacity;
    while (newCap < required)
        newCap *= 2;

    Team *newPtr = (Team *)realloc(*teams, newCap * sizeof(Team));
    if (!newPtr)
        return 0;

    *teams = newPtr;
    *capacity = newCap;
    return 1;
}

int addTeam(Team **teams, int *size, int *capacity, Team candidate){
// check the new team's info is valid
    if (candidate.id <= 0) {
        return 0;
    }
    if (candidate.name[0] == '\0') {
        return 0;
    }
    if (strnlen(candidate.name, sizeof(candidate.name)) >= sizeof(candidate.name)) {
        return 0;
    }
    if (candidate.score < 0 || candidate.missions < 0) {
        return 0;
    }

    // reject if this ID already exists
    if (findTeamIndex(*teams, *size, candidate.id) != -1) {
        return 0;
    }

    // make room for one more team
    if (!ensureCapacity(teams, capacity, *size + 1)) {
        return 0;
    }

    // add the team and update size
    (*teams)[*size] = candidate;
    (*size)++;
    return 1;
}

int recordMission(Team teams[], int size, int id, int points){
    // points must be between 1 and 100
    if (points < 1 || points > 100) {
        return 0;
    }
    // find the team
    int index = findTeamIndex(teams, size, id);
    if (index == -1) {
        return 0;
    }
    // update its score and mission count
    teams[index].score += points;
    teams[index].missions += 1;
    return 1;
}

int deleteTeam(Team teams[], int *size, int id)
{
    int index = findTeamIndex(teams, *size, id);

    if (index == -1) {
        return 0;
    }

    for (int i = index; i < *size - 1; i++) {
        teams[i] = teams[i + 1];
    }

    (*size)--;

    return 1;
}

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

        // Read each field from the line
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

int saveTeams(
    const char *filename,
    const Team teams[],
    int size
)
{
    FILE *file = std::fopen(filename, "w");

    if (file == nullptr) {
        return 0;
    }

    for (int i = 0; i < size; i++) {

        if (std::fprintf(
                file,
                "%d|%s|%d|%d\n",
                teams[i].id,
                teams[i].name,
                teams[i].score,
                teams[i].missions
            ) < 0) {

            std::fclose(file);
            return 0;
        }
    }

    if (std::fclose(file) == EOF) {
        return 0;
    }

    return 1;
}

void readText(const char *prompt, char text[], int limit)
{
    cout << prompt;
    if (fgets(text, limit, stdin) != NULL)
        text[strcspn(text, "\n")] = '\0';
}

int readInt(const char *prompt, int *value)
{
    char line[LINE_LEN], extra;
    cout << prompt;
    if (fgets(line, sizeof line, stdin) == NULL) return 0;
    return sscanf(line, "%d %c", value, &extra) == 1;
}
