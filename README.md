<div id="top"></div>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/The-Tin-Foil-Hat-Society/tin">
    <img src="img/tinfoil-icon.png" alt="Logo" width="128" height="128">
  </a>

<h3 align="center">Tin: a RISC-V Compiler Language</h3>

  <p align="center">
    A procedural programming language written primarily for use on RISC-V hardware.
    <br />
    <a href="https://github.com/The-Tin-Foil-Hat-Society/tin/wiki"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/The-Tin-Foil-Hat-Society/tin/tree/main/examples">Samples</a>
    ·
    <a href="https://github.com/The-Tin-Foil-Hat-Society/tin/issues">Report Bug</a>
    ·
    <a href="https://github.com/The-Tin-Foil-Hat-Society/tin/issues">Request Feature</a>
  </p>
</div>



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

<div align="center">
  
  [![Contributors][contributors-shield]][contributors-url]
  [![Forks][forks-shield]][forks-url]
  [![Stargazers][stars-shield]][stars-url]
  [![Issues][issues-shield]][issues-url]
  [![MIT License][license-shield]][license-url]
  [![Tests][testing-shield]][testing-url]
  
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#test-suite">Test Suite</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

![Code screenshot](https://user-images.githubusercontent.com/12881812/161390756-f98deb3b-ccb1-495d-bccc-56a2cdeaac99.png)

Tin is a structured, imperative and procedural programming language designed around the RISC-V instruction set architecture. It is aimed mainly at applications running on embedded systems, but should be suitable for applications outside of this field.

This is a group project, originally as an assignment for the University of Lincoln. The project itself is a language built for the RISC-V architecture.

<p align="right">(<a href="#top">back to top</a>)</p>



### Built With

* [Flex 2.6.4](https://www.gnu.org/)
* [Bison 3.5.1](https://www.gnu.org/)
* [GCC](https://gcc.gnu.org/)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

Follow the [prerequisites](#prerequisites) and [installation](#installation) guidance to get started, once complete you are able to start making your .tin files.

### Prerequisites

Run the following command to fetch all of the required packages in order to build the toolchain:

  ```sh
  $ sudo apt-get install build-essential flex bison binutils-riscv64-linux-gnu qemu qemu-system-misc qemu-user
  ```

### Installation

1. Clone the repo
   ```sh
   $ git clone https://github.com/The-Tin-Foil-Hat-Society/tin.git
   $ cd tin
   ```
2. Add the standard library to PATH
   ```sh
   $ PATH="$PATH:$(pwd)/std"
   ```
3. Build the project, by default the release version is built
   ```sh
   $ make tin
   ```
   Build the debug version
   ```sh
   $ make tin build=debug
   ```
4. Compile your .tin file
   ```sh
   $ ./build/tin file-name.tin
   ```
5. Run the executable through QEMU
   ```sh
   $ qemu-riscv64 ./file-name
   ```


<p align="right">(<a href="#top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

Call `tin -h` for CLI usage.

This language is currently for the RISC-V architecture, the usage of the language an be seen in the documentation or the [working examples](https://github.com/The-Tin-Foil-Hat-Society/tin/tree/main/examples).

_For more examples, please refer to the [documentation](https://github.com/The-Tin-Foil-Hat-Society/tin/wiki)_.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- Test suite -->
## Testing suite

The following guide assumes you have completed [getting started](#getting-started) and can build .tin files, an important note is to not use the debug version as the clean-up is not designed to handle all the extra files generated or the verbosity outputted in the console. When writing a new test keep in mind that the output of the executable is tested so wherever possible include prints so that information is logged to the console that the tester can then check the differences between.
When running the tester if you want a more verbose output or the expected outputs on failed tests make sure to use the ‘-e’ / ‘--expected’, ‘-v’ / ‘--verbose’ arguments in the command line make note that when using verbose the expected output of failed tests is show also.

### Steps for adding new tests

1.	Create a new .tin file with the code that you wish to test
    ```sh
    $ cd testing/unit-tests directory
    ```
2.	Compile the file and check output is as expected
    ```sh
    $ cd ..
    $ ./build/tin ./testing/unit-tests/file-name.tin
    ```
3.	Run the executable through QEMU and check output is as expected
    ```sh
    $ qemu-riscv64 ./testing/unit-test/file-name
    ```
4.	Store file output
    ```sh
    $ qemu-riscv64 ./testing/unit-test/file-name > ./testing/expected-outputs/file-name.txt
    ```
5.	Run the automated tester
    ```sh
    $ cd testing
    $ python3 test.py
    ```

If the tests pass you will be told that everything worked and if there is a failure for a test, then this will be outputted to the console with which test failed and the expected output of the test if using verbose or expected output mode.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ROADMAP -->
## Roadmap

- [ ] Standard Libraries
- [x] Optimisation
- [x] Implementing #include
- [ ] Memory Tools
- [ ] Non-RISC-V Interpreter

See the [open issues](https://github.com/The-Tin-Foil-Hat-Society/tin/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/YourFeature`)
3. Commit your Changes (`git commit -m 'Add some YourFeature'`)
4. Push to the Branch (`git push origin feature/YourFeature`)
5. Open a Pull Request

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Andrejs Krauze - 25089026@students.lincoln.ac.uk

Hardijs Raubiskis - 25113420@students.lincoln.ac.uk

Kallum Doughty - 25084869@students.lincoln.ac.uk

Project Link: [https://github.com/The-Tin-Foil-Hat-Society/tin](https://github.com/The-Tin-Foil-Hat-Society/tin)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [README Template](https://github.com/othneildrew/Best-README-Template)


<p align="right">(<a href="#top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/The-Tin-Foil-Hat-Society/tin.svg?style=for-the-badge
[contributors-url]: https://github.com/The-Tin-Foil-Hat-Society/tin/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/The-Tin-Foil-Hat-Society/tin.svg?style=for-the-badge
[forks-url]: https://github.com/The-Tin-Foil-Hat-Society/tin/network/members
[stars-shield]: https://img.shields.io/github/stars/The-Tin-Foil-Hat-Society/tin.svg?style=for-the-badge
[stars-url]: https://github.com/The-Tin-Foil-Hat-Society/tin/stargazers
[issues-shield]: https://img.shields.io/github/issues/The-Tin-Foil-Hat-Society/tin.svg?style=for-the-badge
[issues-url]: https://github.com/The-Tin-Foil-Hat-Society/tin/issues
[license-shield]: https://img.shields.io/github/license/The-Tin-Foil-Hat-Society/tin.svg?style=for-the-badge
[license-url]: https://github.com/The-Tin-Foil-Hat-Society/tin/blob/master/LICENSE.txt
[product-screenshot]: images/screenshot.png
[testing-shield]: https://img.shields.io/github/actions/workflow/status/The-Tin-Foil-Hat-Society/tin/READMEtest.yml?branch=main&style=for-the-badge
[testing-url]: https://github.com/The-Tin-Foil-Hat-Society/tin/actions/workflows/READMEtest.yml
