"use strict";
document.addEventListener("DOMContentLoaded", async () => {
  const articleList = document.querySelector("#blogArticles");
  const blogArticleContent = document.querySelector("#blogArticleContent");
  const searchInput = document.querySelector(".searchBox");
  // let articles = [];
  const articlePaths = [
    "/blog/2023/Sep/Growth.md",
    "/blog/2023/Aug/Anime.md",
    "/blog/2023/Jun/Pico.md",
    //
  ];
  const monthsToRetrieve = ["Jun", "Aug", "Sep"];
  // searchInput.disabled = true;

  const fetchMarkdownFile = async (articlePath) => {
    try {
      const resp = await fetch(articlePath);
      if (resp.ok) {
        const markdownText = await resp.text();
        // console.log("markdownText fetch:", markdownText);
        return markdownText;
      } else {
        throw new Error("Failed to fetch .md file");
      }
    } catch (error) {
      console.error(error);
      return null;
    }
  };

  const showArticles = async (articlePaths) => {
    const htmlContent = await Promise.all(
      articlePaths
        .filter((path) => {
          const month = path.split("/")[3];
          return monthsToRetrieve.includes(month);
        })
        .map(async (path, index) => {
          const markdownText = await fetchMarkdownFile(path);
          // console.log("path:", path, index);
          const month = path.split("/")[3];
          // const title = path.split("/")[4].replace(/\.md$/, '');
          const content = marked.parse(markdownText);
          const quickContent = content.split(".")[0];
          // console.log("content:", content);

          return `
            <div class="card">
              <div class="card-container">
                <div>
                  <p>${month}</p>
                </div>
              </div>
              <div>
                <p>${quickContent}...</p>
              </div>
              <div>
              <a href="blogArticle.html?articlePath=${encodeURIComponent(path)}" class="btn read-more-link">Read more</a>
              </div>
            </div>
          `;
        })
    );
    articleList.innerHTML = htmlContent.join("");
  };

  // articlePath query param
  const urlParams = new URLSearchParams(window.location.search);
  const articlePath = urlParams.get("articlePath");

  // load content based on articlePath
  if (articlePath) {
    const markdownText = await fetchMarkdownFile(decodeURIComponent(articlePath));
    const htmlContent = marked.parse(markdownText);
    blogArticleContent.innerHTML = htmlContent;
  }

  // initial render
  showArticles(articlePaths);

  // search render
  searchInput.disabled = false;
  searchInput.addEventListener("input", (e) => {
    const searchStr = e.target.value.toLowerCase();
    const cards = articleList.querySelectorAll('.card');
    cards.forEach((card) => {
      const cardContent = card.querySelector('h2').textContent.toLowerCase();
      if (cardContent.includes(searchStr)) {
        card.style.display = 'block';
      } else {
        card.style.display = 'none';
      }
    });
  });
});

// needs to be publish date - some static var
// const currentDate = new Date(Date.now());

// const dateFormat = {
//   year: "numeric",
//   month: "long",
//   day: "numeric",
// };

// const formattedDate = new Intl.DateTimeFormat("en-US", dateFormat).format(
//   currentDate
// );

// document.getElementById("currentDate").textContent = formattedDate;









/*

old site


// document.addEventListener("DOMContentLoaded", () => {
//   const searchInput = document.querySelector(".searchBox");
//   searchInput.disabled = true;
//   let repoList = document.querySelector("#repos");
//   let repos = [];

//   searchInput.addEventListener("keyup", (e) => {
//     const searchStr = e.target.value.toLowerCase();
//     const filteredRepos = repos.filter((repo) => {
//       return (
//         repo.name.toLowerCase().includes(searchStr) ||
//         (repo.language && repo.language.toLowerCase().includes(searchStr)) ||
//         (repo.description && repo.description.toLowerCase().includes(searchStr))
//       );
//     });
//     showRepos(filteredRepos);
//   });

//   const pullRepos = async () => {
//     try {
//       const api_url = "https://api.github.com/users/cunning-weasel/repos";
//       const res = await fetch(api_url);
//       repos = await res.json();
//       showRepos(repos);
//       searchInput.disabled = false;
//     } catch (err) {
//       console.error(err);
//     }
//   };

//   const showRepos = (repos) => {
//     const htmlString = repos
//       .map((repo) => {
//         return `
//         <div class="card">
//             <div class="card-container">
//                 <div>
//                     <p>${repo.language}</p>
//                 </div>
//                 <h2>${repo.name}</h2>
//             </div>
//                 <div>
//                     <p>${repo.description}</p>
//                 </div>
//                 <div>
//                     <a href="${repo.homepage}" class="btn">Read more</a>
//                 </div>
//         </div>
//           `;
//       })
//       .join("");
//     repoList.innerHTML = htmlString;
//   };
//   pullRepos();
// });
*/