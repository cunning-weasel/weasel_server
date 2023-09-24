"use strict";
// change up for server endpoints
document.addEventListener("DOMContentLoaded", () => {
  const searchInput = document.querySelector(".searchBox");
  searchInput.disabled = true;
  let repoList = document.querySelector("#repos");
  let repos = [];

  searchInput.addEventListener("keyup", (e) => {
    const searchStr = e.target.value.toLowerCase();
    const filteredRepos = repos.filter((repo) => {
      return (
        repo.name.toLowerCase().includes(searchStr) ||
        (repo.language && repo.language.toLowerCase().includes(searchStr)) ||
        (repo.description && repo.description.toLowerCase().includes(searchStr))
      );
    });
    showRepos(filteredRepos);
  });

  const pullRepos = async () => {
    try {
      const api_url = "https://api.github.com/users/cunning-weasel/repos";
      const res = await fetch(api_url);
      repos = await res.json();
      showRepos(repos);
      searchInput.disabled = false;
    } catch (err) {
      console.error(err);
    }
  };

  const showRepos = (repos) => {
    const htmlString = repos
      .map((repo) => {
        return `
        <div class="card">
            <div class="card-container">
                <div>
                    <p>${repo.language}</p>
                </div>
                <h2>${repo.name}</h2>
            </div>
                <div>
                    <p>${repo.description}</p>
                </div>
                <div>
                    <a href="${repo.homepage}" class="btn">Read more</a>
                </div>
        </div>
          `;
      })
      .join("");
    repoList.innerHTML = htmlString;
  };
  pullRepos();
});

// needs to be publish date - some static var
const currentDate = new Date(Date.now());

const dateFormat = {
  year: "numeric",
  month: "long",
  day: "numeric",
};

const formattedDate = new Intl.DateTimeFormat("en-US", dateFormat).format(
  currentDate
);

document.getElementById("currentDate").textContent = formattedDate;

// pass endpoints from above
document.getElementById("content").innerHTML = marked.parse(
  "# Article title goes here\n\nRest of the text and some **bold** sample all written in .md and rendered to html with marked."
);
