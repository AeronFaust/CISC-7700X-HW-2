#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

const int NUM_VARIABLES = 4;

struct Data
{
    double variables[NUM_VARIABLES];
    string label;
};

class KNNModel
{
    private:
        vector<Data> trainingData;

    public:
        //Allows data to be added to the private vector trainingData
        void addData(const double* variables, const string& label)
        {
            Data data;
            copy(variables, variables + NUM_VARIABLES, data.variables);
            data.label = label;
            trainingData.push_back(data);
        }

        //Reads data from a csv file as training data
        void loadTrainingData(const string& filename) 
        {
            ifstream file(filename);
            if (!file.is_open())
            {
                cerr << "Unable to open file " << filename << endl;
                return;
            }

            string line;
            while (getline(file, line))
            {
                stringstream ss(line);
                string token;
                double variables[NUM_VARIABLES];
                string label;

                bool invalidLine = false;//Flag to check invalid line

                for (int i = 0; i < NUM_VARIABLES; ++i) 
                {
                    if (getline(ss, token, ',')) 
                    {
                        try
                        {
                            variables[i] = stod(token);
                        }
                        catch(const invalid_argument& e)
                        {
                            cerr << "warning: Invalid argument in line: " << line << endl;
                            cerr << "Token: " << token << endl;
                            invalidLine = true;
                            break;
                        }
                    } 
                    else 
                    {
                        cerr << "Invalid csv format in line: " << line << endl;
                        return;
                    }
                }

                if (invalidLine)
                {
                    continue;//Skips current line if invalid
                }

                if (getline(ss, label, ',')) 
                {
                    addData(variables, label);
                } 
                else 
                {
                    cerr << "Invalid csv format in line: " << line << endl;
                    return;
                }
            }

            file.close();
        }

        //Takes data read from loadTrainingData as input and outputs predicted label as string
        string predict(const double* inputVariables, int k) 
        {
            vector<pair<double, string>> distances; //pair<distance, label>

            //Calculate distances to all training points
            for (const auto& dataPoint : trainingData) 
            {
                double distance = 0.0;
                for (int i = 0; i < NUM_VARIABLES; ++i) 
                {
                    distance += pow(dataPoint.variables[i] - inputVariables[i], 2);
                }

                distances.push_back({sqrt(distance), dataPoint.label});
            }

            //Sort distances in ascending order
            sort(distances.begin(), distances.end(), [](const auto& a, const auto& b) 
            {
                return a.first < b.first;
            });

            //Count the votes for each label among the k nearest neighbors
            vector<int> labelVotes(trainingData.size(), 0);
            for (int i = 0; i < k; ++i) 
            {
                for (size_t j = 0; j < trainingData.size(); ++j) 
                {
                    if (distances[i].second == trainingData[j].label) 
                    {
                        labelVotes[j]++;
                        break;
                    }
                }
            }

            //Find the label with the most votes
            int maxVotes = 0;
            string predictedLabel;
            for (size_t i = 0; i < labelVotes.size(); ++i)
            {
                if (labelVotes[i] > maxVotes) 
                {
                    maxVotes = labelVotes[i];
                    predictedLabel = trainingData[i].label;
                }
            }

            return predictedLabel;
        }
};

int main()
{
    KNNModel model;

    //Load data from csv file as training data
    model.loadTrainingData("iris.csv");

    double a,b,c,d;//Initialize variables for input 
    string inputString;//initialize string to tokenize
    int k = 1; //Number of neighbors to consider

    cout << "Enter variables separated by commas: ";
    getline(cin, inputString);
    istringstream iss(inputString);

    //Checks if input is valid
    if(iss >> a >> ws && iss.get() == ',' &&
        iss >> b >> ws && iss.get() == ',' &&
        iss >> c >> ws && iss.get() == ',' &&
        iss >> d)
    //Executes prediction if valid
    {
        double input[NUM_VARIABLES] = {a,b,c,d};//Use input from before for an array to get a prediction

        string predictedLabel = model.predict(input, k);
        
        cout << "Predicted Label: " << predictedLabel << endl;
    }
    else//Error case
    {
        cout << "Invalid input format." << endl;
    }

    return 0;
}