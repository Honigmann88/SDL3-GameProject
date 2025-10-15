# SDL3 Game Project

A 2D game project written in C++ using the SDL3 library.  
We are a team of two hobby developers learning game development through this project.

## Project Structure

- `src/` - Contains all source files
  - `main.cpp` - Main application entry point
- Other directories contain build system files and should not be modified directly

## Development Guide

### Prerequisites
- Linux development environment
- C++ compiler (g++/clang)
- SDL3 development libraries
- CMake (if using the provided build system)

### Getting Started
1. Clone the repository
2. Work exclusively in the `src/ directory`
3. Modify `main.cpp` or create new source files in `src/`
4. Test your changes locally before committing

### For Non-Linux Users
If you're using Windows/macOS:
1. Copy the source files to your development environment
2. Set up SDL3 and build tools for your platform
3. Implement and test your changes
4. Copy back the modified source files to this repository

## Versioning System

We use a date-based versioning scheme:  
Format: `v0.Day.Month.Hour.Minute`

Examples:
- `v0.13.10.11.50` for October 13th, 11:50
- `v0.07.12.14.30` for December 7th, 14:30

The final release will be versioned as `v1.0`

### Commit Guidelines
- Use GitHub Desktop for pushes
- Always include a version tag in commit messages
- Describe changes concisely in addition to the version

