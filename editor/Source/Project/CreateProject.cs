using editor.Core;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;

namespace editor.Project
{
    [DataContract]
    public class ProjectTemplate
    {
        [DataMember]
        public string ProjectType { get; set; }

        [DataMember]
        public string ProjectFile { get; set; }

        [DataMember]
        public List<string> Folders { get; set; }

        public byte[] Icon { get; set; }
        public string IconPath { get; set; }
        public byte[] Screenshot { get; set; }
        public string ScreenshotPath { get; set; }
        public string ProjectFilePath { get; set; }
        public string TemplateFilePath { get; set; }
    }

    class CreateProject : ObservableObject
    {
        private readonly string _templatesPath;
        private string _projectName = "New Project";
        private string _projectPath = $@"{Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)}\Fabric Projects\";
        private bool _isValid = false;
        private string _errorMessage;
        private ObservableCollection<ProjectTemplate> _templates = new ObservableCollection<ProjectTemplate>();
        public ReadOnlyObservableCollection<ProjectTemplate> Templates { get; }

        public string ProjectName
        {
            get => _projectName;
            set
            {
                if (_projectName != value)
                {
                    _projectName = value;
                    ValidateProjectPath();
                    OnPropertyChanged();
                }
            }
        }

        public string ProjectPath
        {
            get => _projectPath;
            set
            {
                if (_projectPath != value)
                {
                    _projectPath = value;
                    ValidateProjectPath();
                    OnPropertyChanged();
                }
            }
        }

        public bool IsValid
        {
            get => _isValid;
            set
            {
                if(_isValid != value)
                {
                    _isValid = value;
                    OnPropertyChanged();
                }
            }
        }

        public string ErrorMessage
        {
            get => _errorMessage;
            set
            {
                if (_errorMessage != value)
                {
                    _errorMessage = value;
                    OnPropertyChanged();
                }
            }
        }

        public CreateProject()
        {
            _templatesPath = Environment.GetEnvironmentVariable("FABRIC_PATH", EnvironmentVariableTarget.User);

            _templatesPath = _templatesPath.Remove(_templatesPath.IndexOf("/") + 15); // TODO: When packaged, this line should be removed
            _templatesPath = string.Concat(_templatesPath, @"editor\Project Templates\"); // TODO: When packaged, remove "editor\"

            Templates = new ReadOnlyObservableCollection<ProjectTemplate>(_templates);

            try
            {
                var templateFiles = Directory.GetFiles(_templatesPath, "template.json", SearchOption.AllDirectories);

                foreach(var templateFile in templateFiles)
                {
                    var template = Serializer.Deserialize<ProjectTemplate>(templateFile);

                    template.IconPath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(templateFile), "icon.png"));
                    template.Icon = File.ReadAllBytes(template.IconPath);
                    template.ScreenshotPath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(templateFile), "screenshot.png"));
                    template.Screenshot = File.ReadAllBytes(template.ScreenshotPath);
                    template.ProjectFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(templateFile), template.ProjectFile));
                    template.TemplateFilePath = Path.GetDirectoryName(templateFile);

                    _templates.Add(template);
                }

                ValidateProjectPath();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
                throw;
            }
        }

        private bool ValidateProjectPath()
        {
            var path = ProjectPath;
            if (!Path.EndsInDirectorySeparator(path)) 
                path += @"\";

            path += $@"{ProjectName}\";

            IsValid = false;

            if(string.IsNullOrWhiteSpace(ProjectName.Trim()))
            {
                ErrorMessage = "Type in a project name.";
            }
            else if(ProjectName.IndexOfAny(Path.GetInvalidFileNameChars()) != -1)
            {
                ErrorMessage = "Project name cannot contain invalid characters.";
            }
            else if(string.IsNullOrWhiteSpace(ProjectPath.Trim()))
            {
                ErrorMessage = "Invalid location for project.";
            }
            else if(ProjectPath.IndexOfAny(Path.GetInvalidPathChars()) != -1)
            {
                ErrorMessage = "Project path cannot contain invalid characters.";
            }
            else if(Directory.Exists(path) && Directory.EnumerateFileSystemEntries(path).Any())
            {
                ErrorMessage = "Selected project folder already exists.";
            }
            else
            {
                ErrorMessage = string.Empty;
                IsValid = true;
            }

            return IsValid;
        }

        public string? Create(ProjectTemplate template)
        {
            ValidateProjectPath();

            if (!IsValid)
                return null;

            if (!Path.EndsInDirectorySeparator(ProjectPath))
                ProjectPath += @"\";

            var path = $@"{ProjectPath}{ProjectName}\";

            try
            {
                if(!Directory.Exists(path))
                    Directory.CreateDirectory(path);

                foreach(var folder in template.Folders)
                {
                    Directory.CreateDirectory(Path.GetFullPath(Path.Combine(Path.GetDirectoryName(path), folder)));
                }

                var dirInfo = new DirectoryInfo(path + @".fabric\");
                dirInfo.Attributes |= FileAttributes.Hidden;

                File.Copy(template.IconPath, Path.GetFullPath(Path.Combine(dirInfo.FullName, "icon.png")));
                File.Copy(template.ScreenshotPath, Path.GetFullPath(Path.Combine(dirInfo.FullName, "screenshot.png")));

                var project = new GameProject(ProjectName, path);
                project.Folders = template.Folders;

                Serializer.Serialize(project, path + $"{ProjectName}.fabric");

                CreateSolution(template, path);

                return path;
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
                throw;
            }
        }

        private void CreateSolution(ProjectTemplate template, string path)
        {
            var enginePath = Environment.GetEnvironmentVariable("FABRIC_PATH", EnvironmentVariableTarget.User);

            enginePath = enginePath.Remove(enginePath.IndexOf("/") + 15); // TODO: When packaged, this line should be removed
            var engineIncludePath = string.Concat(enginePath, @"core\include\"); // TODO: When packaged, remove "core\"

            var _0 = ProjectName;
            var _1 = "{" + Guid.NewGuid().ToString().ToUpper() + "}";
            var _2 = engineIncludePath;
            var _3 = enginePath;

            var solution = File.ReadAllText(Path.Combine(template.TemplateFilePath, "GameSolution"));
            solution = string.Format(solution, _0, _1, "{" + Guid.NewGuid().ToString().ToUpper() + "}");
            File.WriteAllText(Path.GetFullPath(Path.Combine(path, $"{_0}.sln")), solution);

            var project = File.ReadAllText(Path.Combine(template.TemplateFilePath, "GameProject"));
            project = string.Format(project, _0, _1, _2, _3);
            File.WriteAllText(Path.GetFullPath(Path.Combine(path, $@"Source\{_0}.vcxproj")), project);
        }
    }
}
