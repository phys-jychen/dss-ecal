#include "Config.hh"

G4int main(G4int argc, char** argv)
{
    Config* config = new Config();

    for (G4int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == std::string("-h") || std::string(argv[i]) == std::string("-help"))
        {
            std::cout << std::endl;
            std::cout << "**********  Help information  **********" << std::endl << std::endl;
            std::cout << "Produce default.yaml: ecal -p" << std::endl;
            std::cout << "Load a YAML file:     ecal -c [file]" << std::endl << std::endl;
            return 0;
        }

        else if (std::string(argv[i]) == std::string("-c"))
            config->Parse(std::string(argv[i + 1]));

        else if (std::string(argv[i]) == std::string("-p"))
        {
            config->Print();
            return 0;
        }
    }

    if (!config->IsLoad())
    {
        std::cout << "No configuration file loaded! Execute \"ecal -h[elp]\" to display help message." << std::endl;
        throw "d";
    }
    else
        config->Run();

    return 0;
}
