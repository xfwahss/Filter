#include "args.hxx"
#include <iostream>


args::Group arguments("arguments");
args::ValueFlag<std::string> gitdir(arguments, "path", "", {"git-dir"});
args::HelpFlag h(arguments, "help", "help", {'h', "help"});
args::PositionalList<std::string> pathsList(arguments, "paths",
                                            "files to commit");

void CommitCommand(args::Subparser &parser) {
    args::ValueFlag<std::string> message(parser, "MESSAGE", "commit message",
                                         {'m'});
    parser.Parse();

    std::cout << "Commit";

    for (auto &&path : pathsList) {
        std::cout << ' ' << path;
    }

    std::cout << std::endl;

    if (message) {
        std::cout << "message: " << args::get(message) << std::endl;
    }
}

int main(int argc, const char **argv) {
    args::ArgumentParser p("git-like parser");
    args::Group commands(p, "commands");
    args::Command add(commands, "add", "add file contents to the index",
                      [&](args::Subparser &parser) {
                          parser.Parse();
                          std::cout << "Add";

                          for (auto &&path : pathsList) {
                              std::cout << ' ' << path;
                          }

                          std::cout << std::endl;
                      });

    args::Command commit(commands, "commit", "record changes to the repository",
                         &CommitCommand);
    args::GlobalOptions globals(p, arguments);

    try {
        p.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << p;
    } catch (args::Error &e) {
        std::cerr << e.what() << std::endl << p;
        return 1;
    }
    return 0;
}