# Contributing to FlavorLang 🌶️

Thanks for considering contributing to FlavorLang! All types of contributions are welcome, including bug fixes, new features, documentation improvements, and ideas.

---

# Table of Contents

- [How to Contribute](#how-to-contribute)
  - [1. Create or Find an Issue](#1-create-or-find-an-issue)
  - [2. Get Authorization](#2-get-authorization)
  - [3. Clone the Repository](#3-clone-the-repository)
  - [4. Create a Branch](#4-create-a-branch)
  - [5. Make Frequent and Descriptive Commits](#5-make-frequent-and-descriptive-commits)
  - [6. Push Changes](#6-push-changes)
  - [7. Open a Pull Request](#7-open-a-pull-request)
  - [8. Code Review and Testing](#8-code-review-and-testing)
- [Coding Standards](#coding-standards)
  - [Commit Standards](#commit-standards)
  - [Code Quality](#code-quality)
- [Code of Conduct](#code-of-conduct)
- [Getting Help](#getting-help)
- [License](#license)

---

## How to Contribute

### 1. Create or Find an Issue

Contributions must be linked to an existing issue. If no relevant issue exists, create one and provide a clear description of the problem or feature.

### 2. Get Authorization

Wait for explicit authorization (via comment or mention) before starting work on any issue.

### 3. Clone the Repository

Clone the repository to your local machine:

```bash
git clone https://github.com/KennyOliver/FlavorLang.git
cd FlavorLang
```

### 4. Create a Branch

Create a new branch based on the `main` branch for your contribution. Use the following naming convention:

```bash
git checkout -b issue-<N>
```

Where `N` is the number of the issue you are addressing. E.g., `issue-123`.

### 5. Make Frequent and Descriptive Commits

Commit your changes often to ensure progress is clear. Avoid committing large bulk changes; keep changes atomic and focused. Use descriptive commit messages in the format:

```
Action: Title
```

**Example:**

```
Fix: Error caused by `function`
```

Provide a detailed description for each commit in the commit body. Mention the issue number using `#N` in the commit description. Enclose any mentions of code (e.g., `my_func`, `my_var`) in backticks for clarity.
Also ensure that any text in the git commit description is done in bullet points.

**Example:**

```
- Did something.
- Some explanation of what was fixed.
- Why the fix was needed / what the issue was.


#123
```

### 6. Push Changes

Push your branch to your forked repository:

```bash
git push origin issue-<N>
```

### 7. Open a Pull Request

Submit a pull request from your branch to the `main` branch of this repository. Include:

- A detailed description of your changes.
- A screenshot demonstrating your code in action.
- Reference to the related issue (e.g., "Closes #N").

### 8. Code Review and Testing

Your pull request will undergo a code review. All GitHub Actions tests must pass before merging. After approval, we will merge your changes into the `main` branch.

## Coding Standards

### Commit Standards

- Use frequent, descriptive commits.
- Follow the format: `Action: Title`.
- Include issue references (e.g., `#N`).
- Enclose code mentions in backticks.
- Use bullet points for git commit descriptions.

### Code Quality

- Write clear and concise comments.
- Ensure consistent formatting (e.g., proper indentation, no trailing spaces).
- Add or update tests when applicable.

## Code of Conduct

All contributors are expected to follow the [Code of Conduct](./CODE_OF_CONDUCT.md). Please be respectful &amp; collaborative.

## Getting Help

If you have questions or need help:

- Check the Issues for similar questions or raise a new one.
- Tag me, @KennyOliver, in the issue or discussion for direct assistance.

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](./LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
