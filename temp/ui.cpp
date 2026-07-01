#include <ftxui/dom/deprecated.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>

int main()
{
    using namespace ftxui;

    // Create a simple document with three text elements.
    auto document =
      vbox({
        hbox(text("♫ Music Player ")) | center | flex,

        separatorHeavy(),

        hbox({
          vbox({
            text("Library") | center | bgcolor(Color::Black) |
              color(Color::White) | border | flex,
          }) |
            size(WIDTH, EQUAL, 50) | border,

          vbox({
            text("QUEUE") | center | bgcolor(Color::Black) |
              color(Color::White) | border | flex,
          }) |
            flex | border,
        }),

        // hbox({
        //   text("left") | bgcolor(Color::Cyan) | color(Color::Black) | border,
        //
        //   text("middle") | border | flex,
        //
        //   text("right") | border,
        // }),
      }) |
      border;

    // Create a screen with full width and height fitting the document.
    auto screen = Screen::Create(Dimension::Full(),        // Width
                                 Dimension::Fit(document)  // Height
    );

    // Render the document onto the screen.
    Render(screen, document);

    // Print the screen to the console.
    screen.Print();
}
