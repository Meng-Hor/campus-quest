//Members:Role
    // Chhay Leng Sok Somnang:saveTeams, deleteTeam
    // Thorng Sovanpichcheata: addTeam, recordMission
    // Ek Vannaro:loadTeams, findTeamIndex
    // Taing Kimhak:addTeam, recordMission
    // Ing Ly Meng Hor:sortLeaderboard, displayTeams
//

// GitHub Repo:https://github.com/Meng-Hor/campus-quest

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

namespace Color {
    const char* Reset       = "\033[0m";
    const char* Red         = "\033[31m";
    const char* Green       = "\033[32m";
    const char* Blue        = "\033[34m";
    const char* DarkGray    = "\033[90m";
    const char* BIBlack     = "\033[3;38;2;0;0;0m";
    const char* BSkyBlue    = "\033[48;2;65;182;230m";
    const char* BPurple     = "\033[48;2;165;94;234m";
}
// The ANSI RGB Formulas
// Foreground (Text Color):\033[38;2;<R>;<G>;<B>m
// Background Color:\033[48;2;<R>;<G>;<B>m
// Reset:\033[0m

#define NAME_LEN 40
#define LINE_LEN 160
#define MENU_WIDTH 75

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
void pause(void);
int readInt(const char *prompt, int *value);
void printError(const string error);
void printSuccess(const char *prompt);
void printMenuBanner(const char *prompt);
void printSubMenuBanner(const char *prompt, bool extraSpace = true);
void printLine(void);
void clearScreen(void);
void printTeamDetails(Team team);

int main(void)
{
    Team *teams = nullptr;
    int size = 0;
    int capacity = 0;
    int choice;
    string error="";
    loadTeams("teams.txt", &teams, &size, &capacity);

    do {
        clearScreen();
        printMenuBanner("Campus Quest Menu"); 
        cout<<Color::Blue<<"  [1] "<<Color::Reset<<"Register new team"<<endl;
        cout<<Color::Blue<<"  [2] "<<Color::Reset<<"Record mission points"<<endl;
        cout<<Color::Blue<<"  [3] "<<Color::Reset<<"Find a team"<<endl;
        cout<<Color::Blue<<"  [4] "<<Color::Reset<<"Remove a team"<<endl;
        cout<<Color::Blue<<"  [5] "<<Color::Reset<<"Show leaderboard"<<endl;
        cout<<Color::Blue<<"  [6] "<<Color::Reset<<"Save and exit"<<endl;
        cout<<endl;
        printLine();
        cout<<endl;
        printError(error);

        if (!readInt("Choose: ", &choice)) {
            // cout << "Invalid menu input.\n";
            error="FAILED: Invalid menu input";
            continue;
        }

        if (choice == 1) {
            error="";
            bool canceled = false;
            Team candidate = {0, "", 0, 0};
            while(true){
                clearScreen();
                printSubMenuBanner("Team Registration");
                printError(error);
                if(!readInt("Enter Team ID (0 to cancel): ", &candidate.id)){
                    error="FAILED: Invalid input";
                    continue;
                }
                if (candidate.id == 0) { canceled = true; break; }
                if (candidate.id < 0) {
                    error="FAILED: ID must be positive integer";
                    continue;
                }
                if(findTeamIndex(teams, size, candidate.id) != -1) {
                    error="FAILED: ID already exist";
                    continue;
                }
                error="";
                break;
            }
            if (canceled) { error = ""; continue; }

            while (true) {
                clearScreen();
                printSubMenuBanner("Team Registration");
                printError(error);
                readText("Enter Team Name (0 to cancel): ", candidate.name, NAME_LEN);
                if (strcmp(candidate.name, "0") == 0) { canceled = true; break; }
                if (candidate.name[0] == '\0') {
                    error="FAILED: Team name cannot be empty";
                    continue;
                }
                error="";
                break; // Input is valid
            }
            if (canceled) { error = ""; continue; }

            if(!addTeam(&teams,&size,&capacity,candidate)){
                error="FAILED: Out of memory";
                continue;
            }else{
                clearScreen();
                printSubMenuBanner("Team Registration");
                printSuccess("SUCCESS: Team added");
                cout<<"\n  ID: "<<candidate.id<<endl;
                cout<<"  Name: "<<candidate.name<<endl;
            }
            pause();
            error="";
        } else if (choice == 2) {
            int id, points;
            bool canceled = false;
            while(true){
                clearScreen();
                printSubMenuBanner("Score Update");
                printError(error);
                if(!readInt("Enter Team ID (0 to cancel): ", &id)) {
                    error="FAILED: Invalid input";
                    continue;
                }
                if (id == 0) { canceled = true; break; }
                int index = findTeamIndex(teams, size, id);
                if (index == -1) {
                    error="FAILED: ID not found";
                    continue;
                }else{
                    printSuccess("SUCCESS: Team found");
                    cout<<endl;
                    printTeamDetails(teams[index]);
                    cout<<endl;
                }
                error="";
                break;
            }
            if (canceled) { error = ""; continue; }

            while(true){
                clearScreen();
                printSubMenuBanner("Score Update");
                // Note: since the screen clears, let's re-print the found team info here so the user sees it while typing points
                int index = findTeamIndex(teams, size, id);
                printSuccess("SUCCESS: Team found");
                cout<<endl;
                printTeamDetails(teams[index]);
                cout<<endl;

                printError(error);
                if(!readInt("Enter Mission Points (0 to cancel): ", &points)) {
                    error="FAILED: Invalid input";
                    continue;
                }
                if (points == 0) { canceled = true; break; }
                if (points < 1 || points > 100) {
                    error="FAILED: Points must be 1 to 100";
                    continue;
                }
                error="";
                break;
            }
            if (canceled) { error = ""; continue; }
            
            if(!recordMission(teams, size, id, points)){
                error="FAILED: Could not update score";
                continue;
            }else{
                clearScreen();
                printSubMenuBanner("Score Update");
                printSuccess("SUCCESS: Point updated");
                cout<<endl;
                int finalIndex = findTeamIndex(teams, size, id);
                if (finalIndex != -1) {
                    printTeamDetails(teams[finalIndex]);
                    cout<<endl;
                }
            }
            pause();
            error="";
        } else if (choice == 3) {
            int id;
            bool canceled = false;
            while(true){
                clearScreen();
                printSubMenuBanner("Find a Team");
                printError(error);
                if(!readInt("Enter Team ID (0 to cancel): ", &id)){
                    error="FAILED: Invalid input";
                    continue;
                }
                if (id == 0) { canceled = true; break; }
                int index = findTeamIndex(teams, size, id);
                if (index == -1) {
                    error="FAILED: ID not found";
                    continue;
                }else{
                    printSuccess("SUCCESS: Team found");
                    cout<<endl;
                    printTeamDetails(teams[index]);
                    cout<<endl;
                }
                error="";
                break;
            }
            if (canceled) { error = ""; continue; }
            pause();
            error="";
        } else if (choice == 4) {
            int id;
            bool canceled = false;
            while(true){
                clearScreen();
                printSubMenuBanner("Remove a Team");
                printError(error);
                if(!readInt("Enter Team ID (0 to cancel): ", &id)){
                    error="FAILED: Invalid input";
                    continue;
                }
                if (id == 0) { canceled = true; break; }
                int index = findTeamIndex(teams, size, id);
                if (index == -1) {
                    error="FAILED: ID not found";
                    continue;
                }
                error="";
                break;
            }
            if (canceled) { error = ""; continue; }

            while(true){
                clearScreen();
                printSubMenuBanner("Remove a Team");
                
                int index = findTeamIndex(teams, size, id);
                printSuccess("SUCCESS: Team found");
                cout<<endl;
                printTeamDetails(teams[index]);
                cout<<endl;

                printError(error);
                char confirm[10];
                readText("Are you sure you want to remove this team? (y/n): ", confirm, sizeof(confirm));
                if (strcmp(confirm, "y") == 0 || strcmp(confirm, "Y") == 0) {
                    error = "";
                    break;
                } else if (strcmp(confirm, "n") == 0 || strcmp(confirm, "N") == 0) {
                    canceled = true;
                    error = "";
                    break;
                } else {
                    error="FAILED: Please enter 'y' or 'n'";
                }
            }
            if (canceled) { error = ""; continue; }

            if(!deleteTeam(teams, &size, id)){
                error="FAILED: Could not remove team";
                continue;
            }else{
                clearScreen();
                printSubMenuBanner("Remove a Team");
                printSuccess("SUCCESS: Team removed");
            }
            pause();
            error="";
        } else if (choice == 5) {
            clearScreen();
            printSubMenuBanner("Leaderboard", false);
            sortLeaderboard(teams, size);
            displayTeams(teams, size);
            pause();
            error="";
        } else if (choice == 6) {
            if (!saveTeams("teams.txt", teams, size))
                cout << "Warning: data could not be saved.\n";
        } else {
            error="FAILED: Option must be 1-6";
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
    printf("  %-6s | %-40s | %-8s | %-8s \n", "ID", "NAME", "SCORE", "MISSIONS");
    for (int i=0; i<size; i++) {
        cout << Color::DarkGray;
        printf("  %-6d | %-40s | %-8d | %-8d \n", 
               teams[i].id, 
               teams[i].name, 
               teams[i].score, 
               teams[i].missions);
        cout << Color::Reset;
    }
    printLine();
    cout<<"  Total: "<<size<<" teams"<<endl;
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
    cout << "  " <<prompt;
    if (fgets(text, limit, stdin) != NULL)
        text[strcspn(text, "\n")] = '\0';
}

void pause(void)
{
    cout << "\n  Press any key to return to main menu...\n";
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

int readInt(const char *prompt, int *value)
{
    char line[LINE_LEN], extra;
    cout << "  " <<prompt;
    if (fgets(line, sizeof line, stdin) == NULL){
        *value=0;
        return 0;
    }
    if(sscanf(line, "%d %c", value, &extra)!=1){
        *value=0;
        return 0;
    }
    return 1;
}

void printError(const string error){
    if(error!="\0"){
        cout<<Color::Red<<"  "<<error<<Color::Reset<<endl<<endl;
    }
}

void printSuccess(const char *prompt){
    cout<<Color::Green<<"  "<<prompt<<Color::Reset<<endl;
}

void printMenuBanner(const char *prompt){
    int width=MENU_WIDTH;
    int text=strlen(prompt);
    int left=(width-text)/2;
    int right=width-text-left;
    for(int i=0; i<width; i++){
        cout<<"-";
        // cout<<Color::BSkyBlue<<"-"<<Color::Reset;
    }
    cout<<endl;
    for(int i=0;i<left;i++){
        cout<<Color::BSkyBlue<<" ";
    }
    cout<<Color::BIBlack<<prompt;
    for(int i=0;i<right;i++){
        cout<<Color::BSkyBlue<<" "<<Color::Reset;
    }
    cout<<endl;
    for(int i=0; i<width; i++){
        cout<<"-";
        // cout<<Color::BSkyBlue<<"-"<<Color::Reset;
    }
    cout<<endl<<endl;
}

void printSubMenuBanner(const char *prompt, bool extraSpace){
    int width=MENU_WIDTH;
    int text=strlen(prompt);
    int left=(width-text)/2;
    int right=width-text-left;
    for(int i=0; i<width; i++){
        cout<<"-";
        // cout<<Color::BSkyBlue<<"-"<<Color::Reset;
    }
    cout<<endl;
    for(int i=0;i<left;i++){
        cout<<Color::BPurple<<" ";
    }
    cout<<Color::BIBlack<<prompt;
    for(int i=0;i<right;i++){
        cout<<Color::BPurple<<" "<<Color::Reset;
    }
    cout<<endl;
    for(int i=0; i<width; i++){
        cout<<"-";
        // cout<<Color::BSkyBlue<<"-"<<Color::Reset;
    }
    if (extraSpace) cout<<endl<<endl;
    else cout<<endl;
}

void printLine(void){
    int width=MENU_WIDTH;
    for(int i=0; i<width; i++){
        cout<<"-";
        // cout<<Color::BWineRed<<"-"<<Color::Reset;
    }
    cout<<endl;
}

void clearScreen(void) {
    cout<<"\033[2J\033[1;1H";
}

void printTeamDetails(Team team) {
    cout<<"    ID:"<<team.id<<endl;
    cout<<"    Name:"<<team.name<<endl;
    cout<<"    Score:"<<team.score<<endl;
    cout<<"    Mission:"<<team.missions<<endl;
}