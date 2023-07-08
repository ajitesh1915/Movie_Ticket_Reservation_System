#include <bits/stdc++.h>

#include <utility>

using namespace std;

#define SUCCESS "success"
#define FAILURE "failure"
#define NONE "none"

/*
 *  Supported Functionalities in Movie Ticket Booking system
 *  AddScreen - adds new screen
 *  ReserveSeat - reserves seats
 *  GetUnreservedSeats - a list of seats which are unreserved yet
 *  SuggestSeats - Based on user preference suggest seats
 *  Invalid - Commands which are not yet supported by Movie Ticket booking system
 */
enum MovieTicketsFunctionalities {
    AddScreen = 0,
    ReserveSeat = 1,
    GetUnreservedSeats = 2,
    SuggestSeats = 3,
    Invalid = 4
};

/*
 * Class provides utilities support to the Movie Ticket Booking system
 */
class Utilities {
public:
    static MovieTicketsFunctionalities FilterFunctionality(string &command) {
        string filterCommand;

        for (char i : command) {
            if (i == ' ')
                break;

            filterCommand += i;
        }

        if (filterCommand == "add-screen")
            return AddScreen;
        else if (filterCommand == "reserve-seat")
            return ReserveSeat;
        else if (filterCommand == "get-unreserved-seats")
            return GetUnreservedSeats;
        else if (filterCommand == "suggest-contiguous-seats")
            return SuggestSeats;

        return Invalid;
    }

    static vector<string> ParseCommand(string &command) {
        vector<string> ArgsList;
        string curArg;

        for (char i : command) {
            if (i == ' ') {
                assert(!curArg.empty());
                ArgsList.push_back(curArg);
                curArg = "";
            }
            else {
                curArg += i;
            }
        }

        assert(!curArg.empty());
        ArgsList.push_back(curArg);

        return ArgsList;
    }

    static bool CheckNumber(string &inp) {
        if (inp.empty())
            return false;

        for (char i : inp) {
            if (i >= '0' && i <= '9')
                continue;
            return false;
        }

        return true;
    }
};


/*
 * Class for each screen added in out Movie ticket booking system
 */
class Screen {
private:
    string screenName;
    int numRows;
    int seatsPerRow;
    vector<vector<bool>> seats;
    vector<bool> isAisleSeat;
    vector<bool> isAisleExtendingLeft;
    vector<bool> isAisleExtendingRight;

  
    bool CheckAvailability(int row, int seatNum) {
        return !seats[row][seatNum];
    }

    void BookSeat(int row, int seatNum) {
        seats[row][seatNum] = true;
    }

    bool IsAisleSeat(int seatNum) {
        return isAisleSeat[seatNum];
    }

    bool IsAisleExtendingLeft(int seatNum) {
        return isAisleExtendingLeft[seatNum];
    }

    bool IsAisleExtendingRight(int seatNum) {
        return isAisleExtendingRight[seatNum];
    }

public:
    Screen(string screenName, int numRows, int seatsPerRow, const vector<int> &aisleSeats) :
        screenName(std::move(screenName)), numRows(numRows), seatsPerRow(seatsPerRow) {
        seats.resize(numRows + 1, vector<bool> (seatsPerRow + 1, false));
        isAisleSeat.resize(seatsPerRow + 1, false);
        isAisleExtendingLeft.resize(seatsPerRow + 1, false);
        isAisleExtendingRight.resize(seatsPerRow + 1, false);

        for (int i : aisleSeats)
            isAisleSeat[i] = true;

        for (int i = 0; i < (int)aisleSeats.size(); i++) {
            if (aisleSeats[i] == 1)
                isAisleExtendingRight[aisleSeats[i]] = true;
            if(aisleSeats[i] == seatsPerRow)
                isAisleExtendingLeft[aisleSeats[i]] = true;
            if (i && aisleSeats[i - 1] == aisleSeats[i] - 1) {
                isAisleExtendingLeft[aisleSeats[i - 1]] = true;
                isAisleExtendingRight[aisleSeats[i]] = true;
            }
            if (i + 1 < (int)aisleSeats.size() && aisleSeats[i + 1] == aisleSeats[i] + 1) {
                isAisleExtendingLeft[aisleSeats[i]] = true;
                isAisleExtendingRight[aisleSeats[i + 1]] = true;
            }
        }

    }

    string ReserveSeats(int row, vector<int> &seatsRequested) {
        // Invalid row number
        if (row <= 0 || row > numRows)
            return FAILURE;

        for (int i : seatsRequested) {
            // Invalid seat number
            if (i <= 0 || i > seatsPerRow)
                return FAILURE;

            if (!CheckAvailability(row, i))
                return FAILURE;
        }

        for (int i : seatsRequested)
            BookSeat(row, i);

        return SUCCESS;
    }

    string GetUnreservedSeats(int row) {
        // Invalid row number
        if (row <= 0 || row > numRows)
            return FAILURE;

        string unreservedSeats;
        bool firstEntry = true;

        for (int i = 1; i <= seatsPerRow; i++) {
            if (CheckAvailability(row, i)) {
                if (!firstEntry)
                    unreservedSeats += ' ';
                unreservedSeats += to_string(i);
                firstEntry = false;
            }
        }

        if (unreservedSeats.empty())
            return NONE;

        return unreservedSeats;
    }

    string SuggestSeats(int row, int seatChoose, int numSeatsRequested) {
        // Invalid row number
        if (row <= 0 || row > numRows)
            return FAILURE;

        // Invalid seat number
        if (seatChoose <= 0 || seatChoose > seatsPerRow)
            return FAILURE;

        if (seatChoose < numSeatsRequested && (seatsPerRow - seatChoose + 1) < numSeatsRequested)
            return NONE;

        bool checkLeft = true;
        bool checkRight = true;

        if (seatChoose >= numSeatsRequested) {
            if (IsAisleSeat(seatChoose) && !IsAisleExtendingLeft(seatChoose)) {
                checkLeft = false;
            }
            else {
                for (int itr = seatChoose; itr >= seatChoose - numSeatsRequested + 1; itr--) {
                    if (!CheckAvailability(row, itr)) {
                        checkLeft = false;
                        break;
                    }

                    if ((itr != seatChoose && IsAisleSeat(itr))) {
                        if (itr == seatChoose - numSeatsRequested + 1 && IsAisleExtendingRight(itr))
                            continue;
                        else {
                            checkLeft = false;
                            break;
                        }
                    }
                }
            }
        }
        else checkLeft = false;

        if (checkLeft) {
            string suggestedSeats;
            bool firstEntry = true;

            for (int itr = seatChoose - numSeatsRequested + 1; itr <= seatChoose ; itr++) {
                if (!firstEntry)
                    suggestedSeats += ' ';
                suggestedSeats += to_string(itr);
                firstEntry = false;
            }

            return suggestedSeats;
        }

        if ((seatsPerRow - seatChoose + 1) >= numSeatsRequested) {
            if (IsAisleSeat(seatChoose) && !IsAisleExtendingRight(seatChoose)) {
                checkRight = false;
            }
            else {
                for (int itr = seatChoose; itr <= seatChoose + numSeatsRequested - 1; itr++) {
                    if (!CheckAvailability(row, itr)) {
                        checkRight = false;
                        break;
                    }
                    if ((itr != seatChoose && IsAisleSeat(itr))) {
                        if (itr == seatChoose + numSeatsRequested - 1 && IsAisleExtendingLeft(itr))
                            continue;
                        else {
                            checkRight = false;
                            break;
                        }
                    }
                }
            }
        }
        else checkRight = false;

        if (checkRight) {
            string suggestedSeats;
            bool firstEntry = true;

            for (int itr = seatChoose; itr <= seatChoose + numSeatsRequested - 1; itr++) {
                if (!firstEntry)
                    suggestedSeats += ' ';
                suggestedSeats += to_string(itr);
                firstEntry = false;
            }

            return suggestedSeats;
        }

        return NONE;
    }
};


/*
 * Movie ticket booking system
 */
class MovieTicketBookingSystem {
private:

    // Stores available screens in the system
    vector <Screen> ScreensInSystem;

    // Stores Screen Ids
    unordered_map<string, int> ScreenIds;


    string AddScreenSystem(string &command) {
        vector<string> ListArgs = Utilities::ParseCommand(command);

        if ((int)ListArgs.size() < 5)
            return FAILURE;
        for (int itr = 2; itr < (int)ListArgs.size(); itr++) {
            if (!Utilities::CheckNumber(ListArgs[itr]))
                return FAILURE;
        }

        string screenName = ListArgs[1];

        // Screen is already added.
        if (ScreenIds.find(screenName) != ScreenIds.end())
            return FAILURE;

        int numRows = stoi(ListArgs[2]);
        int seatsPerRow = stoi(ListArgs[3]);

        // Invalid numbers
        if (numRows <= 0 || seatsPerRow <= 0)
            return FAILURE;

        vector<int> aisleSeats;

        for(int itr = 4; itr < (int)ListArgs.size(); itr++) {
            int curAisleSeat = stoi(ListArgs[itr]);

            // Invalid seat number
            if (curAisleSeat <= 0 || curAisleSeat > seatsPerRow)
                return FAILURE;

            aisleSeats.push_back(curAisleSeat);
        }

        // The aisle is the space for walking with rows of seats on both sides OR
        // with rows of seats on one side and a wall on the other.
        // Checking for if the given seats are valid aisle seats or not

        sort(aisleSeats.begin(), aisleSeats.end());

        for (int i = 0; i < (int)aisleSeats.size(); i++) {
            if (aisleSeats[i] == 1 || aisleSeats[i] == seatsPerRow)
                continue;

            if (i && aisleSeats[i - 1] == aisleSeats[i] - 1)
                continue;

            if (i + 1 < (int)aisleSeats.size() && aisleSeats[i + 1] == aisleSeats[i] + 1)
                continue;

            return FAILURE;
        }

        // Create new screen
        Screen newScreen(screenName, numRows, seatsPerRow, aisleSeats);

        ScreensInSystem.push_back(newScreen);
        ScreenIds[screenName] = (int)ScreensInSystem.size() - 1;

        return SUCCESS;
    }



    string ReserveSeatsSystem(string &command) {
        vector<string> ListArgs = Utilities::ParseCommand(command);

        if ((int)ListArgs.size() < 4)
            return FAILURE;

        for (int itr = 2; itr < (int)ListArgs.size(); itr++) {
            if (!Utilities::CheckNumber(ListArgs[itr]))
                return FAILURE;
        }

        string screenName = ListArgs[1];

        if (ScreenIds.find(screenName) == ScreenIds.end())
            return FAILURE;

        int row = stoi(ListArgs[2]);

        vector<int> seatsRequested;

        for(int itr = 3; itr < (int)ListArgs.size(); itr++)
            seatsRequested.push_back(stoi(ListArgs[itr]));

        return ScreensInSystem[ScreenIds[screenName]].ReserveSeats(row, seatsRequested);
    }
    string GetUnreservedSeatsSystem(string &command) {
        vector<string> ListArgs = Utilities::ParseCommand(command);

        if ((int)ListArgs.size() != 3)
            return FAILURE;

        if (!Utilities::CheckNumber(ListArgs[2]))
            return FAILURE;

        string screenName = ListArgs[1];

        if (ScreenIds.find(screenName) == ScreenIds.end())
            return FAILURE;

        int row = stoi(ListArgs[2]);

        return ScreensInSystem[ScreenIds[screenName]].GetUnreservedSeats(row);
    }
    string SuggestSeatsSystem(string &command) {
        vector<string> ListArgs = Utilities::ParseCommand(command);

        if ((int)ListArgs.size() != 5)
            return FAILURE;
        for (int itr = 2; itr < (int)ListArgs.size(); itr++) {
            if (!Utilities::CheckNumber(ListArgs[itr]))
                return FAILURE;
        }

        string screenName = ListArgs[1];

        // Check for the availability screen in the system
        if (ScreenIds.find(screenName) == ScreenIds.end())
            return FAILURE;

        int numSeatsRequested = stoi(ListArgs[2]);
        int row = stoi(ListArgs[3]);
        int seatChoose = stoi(ListArgs[4]);

        return ScreensInSystem[ScreenIds[screenName]].SuggestSeats(row, seatChoose, numSeatsRequested);
    }

public:

    string ExecuteCommand(string &command) {
        MovieTicketsFunctionalities movieTicketsFunctionality = Utilities::FilterFunctionality(command);

        assert (movieTicketsFunctionality != Invalid);

        if (movieTicketsFunctionality == AddScreen)
            return AddScreenSystem(command);
        else if (movieTicketsFunctionality == ReserveSeat)
            return ReserveSeatsSystem(command);
        else if (movieTicketsFunctionality == GetUnreservedSeats)
            return GetUnreservedSeatsSystem(command);
        return SuggestSeatsSystem(command);
    }
};


int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    // Reading number of commands
    int num_commands;
    cin >> num_commands;

    // Reading excess newline
    string readNewline;
    getline(cin, readNewline);

    // Create new MovieTicketBookingSystem
    MovieTicketBookingSystem movieTicketBookingSystem;

    for (int itr_commands = 0; itr_commands < num_commands; itr_commands++) {
        string command;
        getline(cin, command);

        // Executes Command
        cout << movieTicketBookingSystem.ExecuteCommand(command) << "\n";
    }
}