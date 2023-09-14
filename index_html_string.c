#include "index_html_string.h"

char resp[] = "HTTP/1.0 200 OK\r\n"
              "Server: webserver-c\r\n"
              "Content-type: text/html; charset=utf-8\r\n\r\n"
              "<html>"
              "<head>"
              "<meta name='viewport' content='width=device-width, initial-scale=1' />"
              "<style>"
              "/* CSS styles */"
              "body {"
              "  background-color: #1B2733;"
              "  color: darkgray;"
              "  font-family: 'Courier New', Courier, monospace;"
              "  font-size: 16px;"
              "  text-align: center;"
              "}"
              ".container {"
              "  padding: 1.6rem;"
              "  max-width: 650px; "
              "  margin: 0 auto; "
              "}"
              ".seperator {"
              "  width: auto;"
              "  border-top: 1px solid rgba(236, 233, 230 , 0.15);"
              "  top: 0;"
              "  margin: 1rem;"
              "  position: relative;"
              "}"
              ".card {"
              "  background-color: #1B2733;"
              "  color: #ffffff;"
              "  padding: 1.5em;"
              "  border: 1px solid rgba( 236, 233, 230 , 0.15);"
              "  border-radius: 0.2em;"
              "  box-shadow: 5px 5px 7px rgba(0, 0, 0, 0.15);"
              "  margin-bottom: 1.25em"
              "}"
              ".card:hover {"
              "  transform: translateX(-1.5px);"
              "}"
              ".card h2 {"
              "  margin-top: 0;"
              "  font-size: 20px;"
              "}"
              ".card p {"
              "  margin-bottom: 10px;"
              "}"
              ".card a {"
              "  color: #ffffff;"
              "  text-decoration: none;"
              "  margin-right: 10px;"
              "}"
              ".searchBox {"
              "  margin-bottom: 2em;"
              "  padding: 0.65em 1em;"
              "  background: rgb(27, 39, 51 );"
              "  border: 1px solid rgba(236, 233, 230 , 0.15);"
              "  border-radius: 1.5em;"
              "  color: darkgray"
              "}"
              ".links {"
              "  margin: 20px;"
              "}"
              ".links a {"
              "  display: inline-block;"
              "  margin: 10px;"
              "  background-color: #ffffff;"
              "  color: #1f1f1f;"
              "  padding: 10px 15px;"
              "  text-decoration: none;"
              "  border-radius: 5px;"
              "}"
              "</style>"
              "<title>Paul 'Simba' Bongers</title>"
              "<link rel='icon' href='🦦' type='image/x-icon'>"
              "</head>"
              "<body>"
              "<div class=\"container\">"
              "<p>PAUL SIMBARASHE BONGERS</p>"
              "<p style='padding: 1rem;'>Implemented critical features for internal tooling at Nviso. Managed entire frontend development for an app deployed to over 900 devices at Acoustiguide. Achieved significant milestones at quintly. Was a web manager at Leybold.</p>"
              "<div class='seperator'></div>"
              "<div id=\"projects-sub\">"
              "<input type=\"text\" class=\"searchBox\" placeholder=\"Search articles...\">"
              "<div id=\"repos\"></div>"
              "</div>"
              "<p>get in touch:</p>"
              "<div class=\"links\">"
              "<a href=\"https://github.com/cunning-weasel\" target=\"_blank\">GitHub 🐙</a>"
              "<a href=\"https://www.linkedin.com/in/paul-bongers\" target=\"_blank\">LinkedIn 👨‍💻</a>"
              "<a href=\"mailto:paulbongers22@gmail.com\">Email me 📤</a>"
              "</div>"
              "<p style='color:darkgray;'>{ built with vanilla c, hosted on AWS }</p>"
              "</div>"
              "<script>"
              "\"use strict\";"
              "document.addEventListener('DOMContentLoaded', () => {"
              "  const searchInput = document.querySelector(\".searchBox\");"
              "  searchInput.disabled = true;"
              "  let repoList = document.querySelector(\"#repos\");"
              "  let repos = [];"
              "  searchInput.addEventListener(\"keyup\", (e) => {"
              "    const searchStr = e.target.value.toLowerCase();"
              "    const filteredRepos = repos.filter((repo) => {"
              "      return ("
              "        repo.name.toLowerCase().includes(searchStr) ||"
              "        (repo.language && repo.language.toLowerCase().includes(searchStr)) ||"
              "        (repo.description && repo.description.toLowerCase().includes(searchStr))"
              "      );"
              "    });"
              "    showRepos(filteredRepos);"
              "  });"
              "  const pullRepos = async () => {"
              "    try {"
              "      const api_url = \"https://api.github.com/users/cunning-weasel/repos\";"
              "      const res = await fetch(api_url);"
              "      repos = await res.json();"
              "      showRepos(repos);"
              "      searchInput.disabled = false;"
              "    } catch (err) {"
              "      console.error(err);"
              "    }"
              "  };"
              "  const showRepos = (repos) => {"
              "    const htmlString = repos"
              "      .map((repo) => {"
              "        return ("
              "          `<div class=\"card\">"
              "            <div class=\"card-container\">"
              "              <h2>${repo.name}</h2>"
              "            </div>"
              "            <div>"
              "              <p>${repo.language}</p>"
              "            </div>"
              "            <div>"
              "              <p>${repo.description}</p>"
              "            </div>"
              "            <div>"
              "              <a href=\"${repo.html_url}\" target=\"_blank\">Repository</a>"
              "              <a href=\"${repo.homepage}\" target=\"_blank\">Website</a>"
              "            </div>"
              "          </div>`"
              "        );"
              "      })"
              "      .join(\"\");"
              "    repoList.innerHTML = htmlString;"
              "  };"
              "  pullRepos();"
              "});"
              "</script>"
              "<script src='https://kit.fontawesome.com/a1922851eb.js' crossorigin='anonymous'></script>"
              "</body>"
              "</html>\r\n";
