#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#define NAME_LEN 40
#define LINE_LEN 160

typedef struct
{
    int id;
    char name[NAME_LEN];
    int score;
    int missions;
} Team;

int ensureCapacity(Team **teams, int *capacity, int required);
int main()
{
    Team *teams = NULL;
    int capacity = 0;
    int count = 0;

    int numTeamsToAdd = 3;

    for (int i = 0; i < numTeamsToAdd; i++)
    {
        if (!ensureCapacity(&teams, &capacity, count + 1))
        {
            cout << "Memory allocation failed!" << endl;
            free(teams);
            return 1;
        }

        teams[count].id = i + 1;
        snprintf(teams[count].name, NAME_LEN, "Team %C", 'A' + i);
        teams[count].score = (i + 1) * 100;
        teams[count].missions = i + 2;

        count++;
    }
    cout << "Current capacity: " << capacity << endl;
    for (int i = 0; i < count; i++)
    {
        cout << "ID: " << teams[i].id
             << " | Name: " << teams[i].name
             << " | Score: " << teams[i].score
             << " | Missions: " << teams[i].missions << endl;
    }

    free(teams);
    teams = NULL;

    return 0;
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
